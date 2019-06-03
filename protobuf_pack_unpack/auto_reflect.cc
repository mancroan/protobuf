
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

	// ׼�����ú��ļ�ϵͳ
	google::protobuf::compiler::DiskSourceTree sourceTree;
	// ����ǰ·��ӳ��Ϊ��Ŀ��Ŀ¼ �� project_root �����Ǹ����֣����������Ҫ�ĺϷ�����.
	sourceTree.MapPath("project_root","./");
	// ���ö�̬������.
	google::protobuf::compiler::Importer importer(&sourceTree, NULL);
	// ��̬����protoԴ�ļ��� Դ�ļ���./query.proto .
	importer.Import("project_root/query.proto");
	// ���ڿ��Դӱ���������ȡ���͵�������Ϣ.
	auto descriptor1 = importer.pool()->FindMessageTypeByName("muduo.Query");
	// ����һ����̬����Ϣ����.
	google::protobuf::DynamicMessageFactory factory;
	// ����Ϣ�����д�����һ������ԭ��.
	auto proto1 = factory.GetPrototype(descriptor1);

	// ����һ�����õ���Ϣ.
	auto message1= proto1->New();
	// ������ͨ������ӿڸ��ֶθ�ֵ.
	auto reflection1 = message1->GetReflection();

	auto filed1 = descriptor1->FindFieldByName("id");
	reflection1->SetInt64(message1, filed1,1);
	auto filed2 = descriptor1->FindFieldByName("questioner");
	reflection1->SetString(message1, filed2, "Chen Shuo");
	auto filed3 = descriptor1->FindFieldByName("question");
	reflection1->SetString(message1, filed3, "Running?");

	// ��ӡ����
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

	// ɾ����Ϣ.
	delete message1 ;
	puts("test_read_proto==========================");
}