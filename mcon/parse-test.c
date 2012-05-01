#include "parse.h"

void parse_test(char *spec)
{
	mongo_servers *servers;

	servers = mongo_parse_server_spec(spec);
	mongo_servers_dump(servers);
	mongo_servers_dtor(servers);
}

int main(void)
{
	parse_test("host1:123");
	parse_test("host1:123,host2:123");
	parse_test("mongodb://host1:123,host2:123");
	parse_test("mongodb://derick:test@host1:123");
	parse_test("mongodb://derick:test@host1:123,host2:123");
	parse_test("mongodb://derick:test@host1:123,host2:123/database");
	parse_test("mongodb://derick:test@host1:123,host2/database");
	parse_test("mongodb://derick:test@host1,host2:123/database");
	/* Specifying options */
	parse_test("mongodb://derick:test@host1,host2:123/database?option1=foo");
	parse_test("mongodb://derick:test@host1,host2:123/?option2=bar");
	parse_test("mongodb://derick:test@host1,host2:123?option3=bar");
	parse_test("mongodb://derick:test@host1,host2:123/?option4");
	parse_test("mongodb://derick:test@host1,host2:123/?option5=bar;option6=baz");
	parse_test("mongodb://derick:test@host1,host2:123/?option7=bar&option8=baz");
	parse_test("mongodb://derick:test@host1,host2:123/?option9=bar;option10");
	parse_test("mongodb://derick:test@host1,host2:123/?option11;option12=baz");
	/* Specific options */
	parse_test("mongodb://host1/?replicaSet=testset");
}
