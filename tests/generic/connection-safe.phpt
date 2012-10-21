--TEST--
Connection strings: safe
--SKIPIF--
<?php require dirname(__FILE__) . "/skipif.inc"; ?>
--FILE--
<?php
require_once dirname(__FILE__) . "/../utils.inc";

$hostname = hostname();
$port = port();

$tests = array(
	'safe=false',
	'safe=true',
	'safe=false',
	'safe=true',
);

foreach ( $tests as $key => $value )
{
	$dsn = "mongodb://$hostname:$port/?{$value}";
	echo "\nNow testing $dsn\n";
	$m = new Mongo( $dsn );
	$c = $m->selectCollection( dbname(), "safe-test" );
	$c->drop();
	$c->insert( array( '_id' => $key, 'value' => 'één' ) );
	try
	{
		$c->insert( array( '_id' => $key, 'value' => 'one' ) );
		echo "OK\n";
	}
	catch ( MongoException $e )
	{
		echo "FAIL: ", $e->getMessage(), "\n";
	}
}
?>
--EXPECTF--
Now testing mongodb://%s:%d/?safe=false
OK

Now testing mongodb://%s:%d/?safe=true
FAIL: %s:%d: E11000 duplicate key error index: %s.safe-test.$_id_  dup key: { : 1 }

Now testing mongodb://%s:%d/?safe=false
OK

Now testing mongodb://%s:%d/?safe=true
FAIL: %s:%d: E11000 duplicate key error index: %s.safe-test.$_id_  dup key: { : 3 }
