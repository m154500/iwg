<html>
<header>
<title>B(an){2,5}a</title>

<script>
function update() {
  cb = document.getElementById("replace");
  color = document.getElementById("color");
  if (cb.checked) {
    color.checked = false;
    rreplace();
  }
  else {
    rmatch();
  }
}

function flags() {
  fi = document.getElementById("optioni");
  fx = document.getElementById("optionx");
  fm = document.getElementById("optionm");
  var flags = "";
  if (fi.checked) {
    flags += "i";
  }
  if (fm.checked) {
    flags += "m";
  }
  if (fx.checked) {
    flags += "x";
  }

  document.getElementById("regex3").value = flags;
}

function rreplace() {
  r2 = document.getElementById("regex2");
  r2.disabled = false;
  r2.style.width = "45%";
  r1 = document.getElementById("regex1");
  r1.style.width = "44%";
}

function rmatch() {
  r2 = document.getElementById("regex2");
  r2.disabled = true;
  r2.style.width = "0%";
  r1 = document.getElementById("regex1");
  r1.style.width = "88%";
}

function process_regex() {
  xmlhttp = new XMLHttpRequest();
  xmlhttp.open("POST","process.php",true);

  var params = "";
  params += "flags=" + document.getElementById("regex3").value;
  params += "&r1=" + encodeURIComponent(document.getElementById("regex1").value);
  params += "&r2=" + encodeURIComponent(document.getElementById("regex2").value);
  params += "&replace=" + (document.getElementById("replace").checked?"1":"0");
  params += "&color=" + (document.getElementById("color").checked?"1":"0");
  params += "&LOGGING=" + (document.getElementById("logging").checked?"1":"0");
  params += "&search=" + encodeURIComponent(document.getElementById("input").value);

  console.log(params);

  xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
  xmlhttp.setRequestHeader("Content-length",params.length);
  xmlhttp.setRequestHeader("Connection","close");
  xmlhttp.send(params);

  xmlhttp.onreadystatechange = function() {
      if(xmlhttp.readyState == 4 && xmlhttp.status == 200) {
          var result = xmlhttp.responseText;
          if (result != "") {
            document.getElementById("output").innerHTML = result;
          }
          else {
            document.getElementById("output").innerHTML = "<font color=red>&lt;no matches&gt;</font>";
          }
      }
  }
}

function history() {
  xmlhttp = new XMLHttpRequest();
  xmlhttp.open("POST","history.php",true);

  if ((document.getElementById("input").value == "") ||
    (document.getElementById("input").value == "and then enter your data here!")) {

    var params = "";
    params += "&r1=" + encodeURIComponent(document.getElementById("regex1").value);
    params += "&r2=" + encodeURIComponent(document.getElementById("regex2").value);
    params += "&replace=" + (document.getElementById("replace").checked?"1":"0");

    console.log(params);

    xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
    xmlhttp.setRequestHeader("Content-length",params.length);
    xmlhttp.setRequestHeader("Connection","close");
    xmlhttp.send(params);

    xmlhttp.onreadystatechange = function() {
        if(xmlhttp.readyState == 4 && xmlhttp.status == 200) {
            document.getElementById("input").value = xmlhttp.responseText;
        }
    }
  }
}
</script>

</header>
<body>
<h1>The Premiere Regex Testing Website!!!!!</h1>


<div id="left" style="float:left; width: 48%; margin: 1%;">
  <div id="options" style="float:left; width: 50%;">
    <form action="#">
    <input type="checkbox" id="optioni" onchange="flags()">
    <code>/i : case insensitive</code>
    <br>
    <input type="checkbox" id="optionm" onchange="flags()">
    <code>/m : multi-line matching</code>
    <br>
    <input type="checkbox" id="optionx" onchange="flags()">
    <code>/x : ignore whitespace in regex</code>
    </form>
  </div>
  <div id="moreoptions" style="float:right; width: 50%;">
    <form action="#">
    <input id="replace" type="checkbox" onchange="update()">
    <code>run replacement, not matching</code>
    <br>
    <input type="checkbox" id="color" onchange="update()">
    <code>run in color mode (matching only)</code>
    <br>
    <input type="checkbox" id="logging">
    <code>save query for later use</code>
    </form>
  </div>

  <input type="text" id="regex1" style="width:88%; margin:-1" value="enter your regex here!" onchange="history()"></input>
  <input type="text" disabled id="regex2" style="width:0%; margin:-1" value="" onchange="history()"></input>
  <input type="text" disabled id="regex3" style="width:10%; margin:-1" value=""></input>
  <br>
  <textarea id="input" style="width: 100%" rows=10>and then enter your data here!</textarea>
  <br>
  <input name="run" type=submit value="Run Regex" onclick="process_regex()"></input>
</div>
<div id="right" style="float:right; width: 48%; margin: 1%;">
  <div id="output" style="width: 100%; padding: 2px; height:270px; border:1px solid; border-color: #999999; border-radius: 4px;">
    output will go here
  </div>
</div>

</body>

<?php
if (isset($_GET['key'])) {
  $fh = fopen("./key","r");
  if (strcmp($_GET['key'],fread($fh,filesize('./key'))) == 0) {
    echo "HELLO SERVICE POLLER";
  }
}
?>
