<?php


$search_string  = $_POST['search'];
 unset($_POST['search']);
$match          = $_POST['r1'];
 unset($_POST['r1']);
$replace        = $_POST['r2'];
 unset($_POST['r2']);
$flags          = $_POST['flags'];
 unset($_POST['flags']);
$do_replace     = $_POST['replace'];
 unset($_POST['replace']);
$do_color       = $_POST['color'];
 unset($_POST['color']);

foreach ($_POST as $k => $v) {
  putenv("$k=$v");
}

if (!$do_color) {
  if (!$do_replace) {
    preg_match_all("~$match~$flags",$search_string,$out,PREG_SET_ORDER);
    foreach ($out as &$b) {
      echo "$b[0]<br>";
    }
  } else {
    echo preg_replace("~$match~$flags",$replace,$search_string);
  }
} else {
  $f = tempnam("/tmp/",".color");
  $fh = fopen($f,"w");
  fwrite($fh,$search_string);
  fclose($fh);
  $match = escapeshellarg($match.'|$');
  $res = str_replace("\n","<br>",`grep -aP --color=always $match $f`);
  $res = str_replace("\x1b[01;31m\x1b[K", "<font color=green>", $res);
  $res = str_replace("\x1b[m\x1b[K", "</font>", $res);
  echo $res;
  unlink($f);
}

function delete_oldest($arr) {
  $oldest_time = time();
  $oldest_file = "";
  foreach ($arr as $file) {
    $t = filectime("./log/$file");
    if ($t < $oldest_time) {
      $oldest_time = $t;
      $oldest_file = $file;
    }
  }
  unlink("./log/$oldest_file");
}

if (getenv('LOGGING')) {
  $safen = str_replace(str_split('+/'),str_split('-_'),base64_encode($match));
  if ($do_replace) {
    $safen .= ".".str_replace(str_split('+/'),str_split('-_'),base64_encode($match));
  }
  $fh = fopen("./log/$safen","w");
  fwrite($fh,substr($search_string,0,2048));
  fclose($fh);
  chmod("./log/$safen",1400);
  $arr = scandir("./log");
  if (count($arr) > 2048) {
    for ($i = count($arr); $i > 2048; $i = count($arr)) {
      delete_oldest($arr);
      $arr = scandir("./log");
    }
  }
}

?>
