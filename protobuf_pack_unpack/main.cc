#include "codec.h"

void codec_test();
void descriptor_test();
void test_read_proto();
void test_dynamic_proto();

int main()
{
	//测试描述信息
	//descriptor_test();

	//测试打包解包
	codec_test();

	//直接解析proto 文件模式 不用编译proto文件
	//test_dynamic_proto();

	google::protobuf::ShutdownProtobufLibrary();

	system("pause");
	return 0;
}

