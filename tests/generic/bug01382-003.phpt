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
foreach ($cur as $arr) {
	$info = $cur->info(); echo @$info['at'], ' - ', @$info['numReturned'], "\n";
	var_dump($arr['_id']);
}

?>
--EXPECT--
0 - 3
string(5) "test1"
1 - 3
string(5) "test2"
2 - 3
string(5) "test3"
