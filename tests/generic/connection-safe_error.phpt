--TEST--
Connection strings: safe (errors)
--SKIPIF--
<?php require dirname(__FILE__) . "/skipif.inc"; ?>
--FILE--
<?php
require_once dirname(__FILE__) . "/../utils.inc";

$hostname = hostname();
$port = port();

$tests = array(
	'safe=False',
	'safe=True',
	'safe=0',
	'safe=1',
	'safe=-1',
);

foreach ( $tests as $key => $value )
{
	$dsn = "mongodb://$hostname:$port/?{$value}";
	echo "\nNow testing $dsn\n";
	try
	{
		$m = new Mongo( $dsn );
		echo "OK\n";
	}
	catch ( MongoException $e )
	{
		echo "FAIL: ", $e->getMessage(), "\n";
	}
}
?>
--EXPECTF--
Now testing mongodb://%s:%d/?safe=False
FAIL: The value of 'safe' needs to be either 'true' or 'false'

Now testing mongodb://%s:%d/?safe=True
FAIL: The value of 'safe' needs to be either 'true' or 'false'

Now testing mongodb://%s:%d/?safe=0
FAIL: The value of 'safe' needs to be either 'true' or 'false'

Now testing mongodb://%s:%d/?safe=1
FAIL: The value of 'safe' needs to be either 'true' or 'false'

Now testing mongodb://%s:%d/?safe=-1
FAIL: The value of 'safe' needs to be either 'true' or 'false'
