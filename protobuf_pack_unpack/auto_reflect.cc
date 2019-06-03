
#include <iostream>
#include <exception>  
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include "codec.h"

void test_read_proto()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	//muduo::Query query;
	//query.set_id(1);
	//query.set_questioner("Chen Shuo");
	//query.add_question("Running?");
	//std::string transport = encode(query);

	// 准备配置好文件系统
	google::protobuf::compiler::DiskSourceTree sourceTree;
	// 将当前路径映射为项目根目录 ， project_root 仅仅是个名字，你可以你想要的合法名字.
	sourceTree.MapPath("project_root","./");
	// 配置动态编译器.
	google::protobuf::compiler::Importer importer(&sourceTree, NULL);
	// 动态编译proto源文件。 源文件在./query.proto .
	importer.Import("project_root/query.proto");
	// 现在可以从编译器中提取类型的描述信息.
	auto descriptor1 = importer.pool()->FindMessageTypeByName("muduo.Query");
	// 创建一个动态的消息工厂.
	google::protobuf::DynamicMessageFactory factory;
	// 从消息工厂中创建出一个类型原型.
	auto proto1 = factory.GetPrototype(descriptor1);

	// 构造一个可用的消息.
	auto message1= proto1->New();
	// 下面是通过反射接口给字段赋值.
	auto reflection1 = message1->GetReflection();

	auto filed1 = descriptor1->FindFieldByName("id");
	reflection1->SetInt64(message1, filed1,1);
	auto filed2 = descriptor1->FindFieldByName("questioner");
	reflection1->SetString(message1, filed2, "Chen Shuo");
	auto filed3 = descriptor1->FindFieldByName("question");
	reflection1->SetString(message1, filed3, "Running?");

	// 打印看看
	//std::cout << message1->DebugString();

	std::string transport = encode(*message1);
	//print(transport);

	int32_t be32 = 0;
	std::copy(transport.begin(), transport.begin() + sizeof be32, reinterpret_cast<char*>(&be32));
	int32_t len = ::ntohl(be32);
	assert(len == transport.size() - sizeof(be32));

	// network library decodes length header and get the body of message
	std::string buf = transport.substr(sizeof(int32_t));
	assert(len == buf.size());

	auto message2 = decode(buf);
	assert(message2 != NULL);
	message2->PrintDebugString();
	assert(message2->DebugString() == message1->DebugString());
	delete message2;

	buf[buf.size() - 6]++;  // oops, some data is corrupted
	auto badMsg = (decode(buf));
	assert(badMsg == NULL);

	// 删除消息.
	delete message1 ;
	puts("test_read_proto==========================");
}