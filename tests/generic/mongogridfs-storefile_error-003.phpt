--TEST--
MongoGridFS::storeFile() throws exception for nonexistent file
--SKIPIF--
<?php require "tests/utils/standalone.inc";?>
--FILE--
<?php
require_once "tests/utils/server.inc";
$mongo = mongo_standalone();
$db = $mongo->selectDB(dbname());

$gridfs = $db->getGridFS();

try {
    $gridfs->storeFile('/does/not/exist');
    var_dump(false);
} catch (MongoGridFSException $e) {
    var_dump(true);
}
--EXPECT--
bool(true)
