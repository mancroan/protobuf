#include "codec.h"

void codec_test();
void descriptor_test();
void test_read_proto();
void test_dynamic_proto();

int main()
{
	//����������Ϣ
	//descriptor_test();

	//���Դ�����
	codec_test();

	//ֱ�ӽ���proto �ļ�ģʽ ���ñ���proto�ļ�
	//test_dynamic_proto();

	google::protobuf::ShutdownProtobufLibrary();

	system("pause");
	return 0;
}

