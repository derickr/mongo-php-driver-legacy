--TEST--
Test for bug PHP-266: segfault when connection string refers to unknown replica set member
--SKIPIF--
<?php require_once dirname(__FILE__) ."/skipif.inc"; ?>
<?php if (!$STANDALONE_HOSTNAME) { exit("skip Needs a standalone server too"); } ?>
--FILE--
<?php require_once dirname(__FILE__) ."/cfg.inc"; ?>
<?php
$host = hostname();
$port = port();
$ip   = gethostbyname($host);

$host2  = hostname("STANDALONE");
$port2 = port("STANDALONE");
$ip2   = gethostbyname($host2);

try {
	$m = new Mongo("$ip:$port,$ip2:$port2", array("replicaSet" => true));
} catch (MongoConnectionException $e) {
	var_dump( $e->getMessage() );
}
?>
--EXPECTF--
string(102) "While discovering nodes, is_master returned with an error for %s:%d: Not a replicaset member"
