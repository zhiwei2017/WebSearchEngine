// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>

#include <boost/asio.hpp>
#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <exception>
#include "./SearchServer.h"
#include "./Record.h"

using boost::asio::ip::tcp;
using std::string;
using std::vector;

SearchServer::SearchServer(const string& filename, uint16_t port)
: _q(3), _port(port), _io_service(), _acceptor(_io_service,
        tcp::endpoint(tcp::v4(), _port)) {
  _q.readFromFile(filename);
}

// _____________________________________________________________________________
void SearchServer::run() {
  while (true) {
    try {
      std::cout << "Waiting for query at port " << _port << " ..." << std::endl;
      tcp::socket client(_io_service);
      _acceptor.accept(client);
      // Get the request string.
      std::vector<char> requestBuffer(1000);
      boost::system::error_code read_error;
      client.read_some(boost::asio::buffer(requestBuffer), read_error);
      std::string request(requestBuffer.size(), 0);
      std::copy(requestBuffer.begin(), requestBuffer.end(), request.begin());
      size_t pos = request.find('\r');
      if (pos != std::string::npos) { request.resize(pos); }
      if (request == "GET / HTTP/1.1") {
        request = "GET /search.html HTTP/1.1";
      }
      std::cout << "Request string is \"" << request << "\"" << std::endl;
      string response = getHttpResponse(request);
      boost::system::error_code write_error;
      boost::asio::write(client, boost::asio::buffer(response),
      boost::asio::transfer_all(), write_error);
    }
    catch(const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

// _____________________________________________________________________________
string SearchServer::getDummyResult(const string& query) {
  string json = "{\r\n\"matches\": [";
  Recordlist res = _q.findMatches(query, query.size() / 4, 10);
  for (size_t i = 0; i < res.size(); ++i) {
    string nameAndYear = res[i]._Name + " (" + res[i]._Year + ")";
    string pictureUrl = "https://usercontent.googleapis.com/freebase/v1/image/" +
    res[i]._MID;
    json += "{\r\n\"item\": [";
    json += "\"" + escapeForJson(nameAndYear) + "\"";
    json += ',';
    json += "\r\n";
    json += "\"" + escapeForJson(pictureUrl) + "\"";
    json += "\r\n";
    json += "]\r\n}";
    if (i + 1 < res.size()) {
      json += ',';
    }
    json += "\r\n";
  }
  json += "]\r\n}";
  return json;
}

// _____________________________________________________________________________
string SearchServer::getHttpResponse(const string& request) {
  string contentString;
  std::smatch match;
  std::regex_match(request, match, std::regex("^GET /(.*) HTTP/1.1"));
  if (match.size() == 0) {
    std::cerr << "Request is unknown." << std::endl;
    return contentString;
  }
  string query = decodeFromHttp(match[1]);
  string statusString = "HTTP/1.1 200 OK";
  string contentType = "text/plain";
  if (startsWith(query, "?")) {
    contentString = getDummyResult(_q.split(query.substr(3)));
    std::cout << "Answer: " << contentString << std::endl;
    contentType = "application/json";
  } else {
    std::cout << "Needed file: \"" << query << "\" ... \n";
    std::ifstream in(query.c_str(), std::ios_base::in);
    if (!in) {
      statusString = "HTTP/1.1 404 NOT FOUND";
      contentString = "404 NOT FOUND";
    } else {
      // File into string
      contentString = string((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
      // Set content type
      if (endsWith(query, ".html")) {
        contentType = "text/html";
      } else if (endsWith(query, ".css")) {
        contentType = "text/css";
      } else if (endsWith(query, ".js")) {
        contentType = "application/javascript";
      }
    }
  }
  std::ostringstream oss;
  oss << statusString << "\r\n"
  << "Content-Length: " << contentString.size() << "\r\n"
  << "Content-Type: " << contentType << "\r\n"
  << "\r\n";

  return oss.str() + contentString;
}

// ____________________________________________________________________________
string SearchServer::escapeForJson(const string& orig) {
  string result;
  for (size_t i = 0; i < orig.size(); ++i) {
    if (orig[i] == 0) result += "\\0";
    else if (orig[i] == 8) result += "\\b";
    else if (orig[i] == 9) result += "\\t";
    else if (orig[i] == 10) result += "\\n";
    else if (orig[i] == 11) result += "\\v";
    else if (orig[i] == 12) result += "\\f";
    else if (orig[i] == 34 || orig[i] == 92) result += '\\' + orig[i];
    else result += orig[i];
  }
  return result;
}

// ____________________________________________________________________________
bool SearchServer::startsWith(const string& text, const char* prefix,
                              size_t prefixSize) {
  if (prefixSize > text.size()) {
    return false;
  }
  for (size_t i = 0; i < prefixSize; ++i) {
    if (text[i] != prefix[i]) {
      return false;
    }
  }
  return true;
}
// ____________________________________________________________________________
bool SearchServer::startsWith(const string& text, const string& prefix) {
    return startsWith(text, prefix.data(), prefix.size());
}
// ____________________________________________________________________________
bool SearchServer::startsWith(const string& text, const char* prefix) {
    return startsWith(text, prefix, std::char_traits<char>::length(prefix));
}

// ____________________________________________________________________________
bool SearchServer::endsWith(const string& text, const char* suffix,
                            size_t suffixSize) {
  if (suffixSize > text.size()) {
    return false;
  }
  for (size_t i = 0; i < suffixSize; ++i) {
    if (text[text.size() - (i + 1)] != suffix[suffixSize - (i + 1)]) {
      return false;
    }
  }
  return true;
}
// ____________________________________________________________________________
bool SearchServer::endsWith(const string& text, const string& suffix) {
    return endsWith(text, suffix.data(), suffix.size());
}
// ____________________________________________________________________________
bool SearchServer::endsWith(const string& text, const char* suffix) {
    return endsWith(text, suffix, std::char_traits<char>::length(suffix));
}

// ____________________________________________________________________________
vector<string> SearchServer::split(const string& orig, const char sep) {
  vector<string> res;
  if (orig.size() > 0) {
    size_t from = 0;
    size_t sepIndex = orig.find(sep);
    while (sepIndex != string::npos) {
      res.push_back(orig.substr(from, sepIndex - from));
      from = sepIndex + 1;
      sepIndex = orig.find(sep, from);
    }
    res.push_back(orig.substr(from));
  }
  return res;
}

// ____________________________________________________________________________
string SearchServer::decodeFromHttp(const string& encoded) {
  string decoded;
  size_t i = 0;
  while (i < encoded.size()) {
    if (encoded[i] == '%' && i + 2 < encoded.size()) {
      decoded += ' ';
      i += 3;
    } else {
      decoded += encoded[i];
      ++i;
    }
  }
  return decoded;
}
