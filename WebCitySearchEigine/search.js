$(document).ready(function(){
  $("#query").keyup(function() {
    processQuery($("#query").val());
  });

  $("#hit").keyup(function(event) {
    console.log("hit = " + $("#hit").val());
    document.cookie = "hit=" + $("#hit").val();
  });
});

function processQuery(query) {
  console.log(query.term);
  var hit = document.cookie.split("=")[1];
  if (hit == undefined) hit = 10;
  $.getJSON("/?q=" + query + "&hit=" + hit, function(result) {
    res = "<div><p><span><table><tr><th id=\"title\">City Name</th><th id=\"region\">Reigon</th><th id=\"population\">Population</th></tr></table></span></p></div>";
    for (i = 0; i < result.matches.length; i++) {
      res += "<div><p><span><table><tr><td id=\"title\"><a href=\"javascript:void(0);\"onClick=window.open(\"" + result.matches[i].item[1] + "\",\"Ratting\",\"width=850,height=570,status=0,scrollbars=1\");>" + result.matches[i].item[0] + "</a></td><td id=\"region\">" + result.matches[i].item[2] + "</td><td id=\"population\">" + result.matches[i].item[3] + "</td></tr></table></span></p></div>";
    }
    res += "</table>";
    $("#answer").html(res);
  });
}

