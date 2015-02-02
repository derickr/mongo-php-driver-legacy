--TEST--
Test for PHP-1382: Last document missing
--SKIPIF--
<?php require_once "tests/utils/standalone.inc" ?>
--FILE--
<?php
require_once "tests/utils/server.inc";

$host = MongoShellServer::getStandaloneInfo();
$m = new MongoClient($host);
$d = $m->selectDb(dbname());
$c = $m->selectCollection(dbname(), collname(__FILE__));
$c->drop();

$c->save(array('_id' => 'test1'));
$c->save(array('_id' => 'test2'));
$c->save(array('_id' => 'test3'));
$cur = $c->find(array(), array('_id'));
var_dump(iterator_to_array($cur));

?>
--EXPECT--
array(3) {
  ["test1"]=>
  array(1) {
    ["_id"]=>
    string(5) "test1"
  }
  ["test2"]=>
  array(1) {
    ["_id"]=>
    string(5) "test2"
  }
  ["test3"]=>
  array(1) {
    ["_id"]=>
    string(5) "test3"
  }
}
