<?php

$match          = $_POST['r1'];
 unset($_POST['r1']);
$replace        = $_POST['r2'];
 unset($_POST['r2']);
$do_replace     = $_POST['replace'];
 unset($_POST['replace']);

foreach ($_POST as $k => $v) {
  putenv("$k=$v");
}


$safen = str_replace(str_split('+/'),str_split('-_'),base64_encode($match));
if ($do_replace) {
  $safen .= ".".str_replace(str_split('+/'),str_split('-_'),base64_encode($match));
}
if (file_exists("./log/$safen")) {
  echo `head -c 2048 ./log/$safen`;
}
echo "";

?>
