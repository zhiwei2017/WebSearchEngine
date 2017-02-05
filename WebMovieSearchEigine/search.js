// This is just a very short core example.
// Your solutions are certainly much nicer.

$(document).ready(function(){
  $("#query").keyup(function() {
    processQuery($("#query").val()) 
  });
});

function processQuery(query) {
  console.log(query);
  $.getJSON("/?q=" + query, function(result) {
    res = "<table>";
    for (i = 0; i < result.matches.length; i++) {
      res += "<tr><td id=\"pic\"><img src=\"" + result.matches[i].item[1] + "\"/>" + "</td><td id=\"title\">" + result.matches[i].item[0] + "</td></tr>";
    }
    res += "</table>";
    $("#answer").html(res);
  });
}

