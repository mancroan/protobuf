
#include <iostream>
#include <exception>
#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

#include "codec.h"

void unSerialize(std::string &transport);

void test_dynamic_proto()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	// 准备配置好文件系统
	google::protobuf::compiler::DiskSourceTree sourceTree;
	// 将当前路径映射为项目根目录 ， project_root 仅仅是个名字，你可以你想要的合法名字.
	sourceTree.MapPath("project_root","./");
	// 配置动态编译器.
	google::protobuf::compiler::Importer importer(&sourceTree, NULL);
	// 动态编译proto源文件。 源文件在./query.proto .
	importer.Import("project_root/query.proto");
	// 现在可以从编译器中提取类型的描述信息.
	const google::protobuf::Descriptor *descriptor = importer.pool()->FindMessageTypeByName("muduo.Query");
	// 创建一个动态的消息工厂.
	google::protobuf::DynamicMessageFactory factory;
	// 从消息工厂中创建出一个类型原型.
	const google::protobuf::Message *proto = factory.GetPrototype(descriptor);
	// 构造一个可用的消息.
	google::protobuf::Message *message= proto->New();
	// 下面是通过反射接口给字段赋值.
	const google::protobuf::Message::Reflection  *reflection = message->GetReflection();

	const google::protobuf::FieldDescriptor *filed1 = descriptor->FindFieldByName("id");
	reflection->SetInt64(message, filed1,1);
	const google::protobuf::FieldDescriptor *filed2 = descriptor->FindFieldByName("questioner");
	reflection->SetString(message, filed2, "WHA");
	const google::protobuf::FieldDescriptor *filed3 = descriptor->FindFieldByName("question");
	reflection->SetString(message, filed3, "Running?");
	// 打印看看
	std::cout << message->DebugString();

	std::string transport = encode(*message); //打包
	unSerialize(transport);
	delete message;
}

void unSerialize(std::string &transport)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	google::protobuf::compiler::DiskSourceTree sourceTree;
	google::protobuf::compiler::Importer importer(&sourceTree, NULL);
	sourceTree.MapPath("project_root","./");
	importer.Import("project_root/query.proto");
	google::protobuf::DynamicMessageFactory factory;

	int32_t be32 = 0;
	std::copy(transport.begin(), transport.begin() + sizeof be32, reinterpret_cast<char*>(&be32));
	int32_t len1 = ::ntohl(be32);
	assert(len1 == transport.size() - sizeof(be32));

	// network library decodes length header and get the body of message
	std::string buf = transport.substr(sizeof(int32_t));
	assert(len1 == buf.size());

	google::protobuf::Message *message = decode2(buf, importer, factory);
	assert(message != NULL);
	message->PrintDebugString();
	delete message;

	buf[buf.size() - 6]++;  // oops, some data is corrupted
	google::protobuf::Message* badMsg = (decode(buf));
	assert(badMsg == NULL);
}

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;

void testDynamic()
{
	DiskSourceTree sourceTree;
	sourceTree.MapPath("", "./");
	Importer importer(&sourceTree, NULL);
	importer.Import("my.proto");
	const Descriptor *descriptor = importer.pool()->FindMessageTypeByName("mymsg");
	DynamicMessageFactory factory;
	const Message *message = factory.GetPrototype(descriptor);
	const FieldDescriptor *field = NULL;
	field = descriptor->FindFieldByName("len");
	const FieldDescriptor *field1 = NULL;
	field1 = descriptor->FindFieldByName("typ");
	Message *msg = message->New();
	const Reflection *reflection = msg->GetReflection();
	reflection->SetInt32(msg, field, 1);
	reflection->SetInt32(msg, field1, 2);
	std::string sout;
	msg->SerializeToString(&sout);
	delete msg;

	Message *msg2 = message->New();
	msg->ParseFromString(sout);
	const Reflection *reflection2 = msg2->GetReflection();
	const FieldDescriptor *field2 = NULL;
	field2 = descriptor->FindFieldByName("len");
	int len = reflection2->GetInt32(*msg2, field2);
	field2 = descriptor->FindFieldByName("typ");
	int typ = reflection2->GetInt32(*msg2, field2);
	msg2->PrintDebugString();

	delete msg2;
}
