#pragma once

// ҵ����ص�һЩ���Ͷ���

#pragma region namespace
namespace im {
#pragma endregion

// ��ǰ�ͻ�����ʹ�õĿͻ�������
enum class EClientType {
  win = 0,
  mac,
  linux,
  android,
  ios
};

// ����client����
#ifdef _WIN32
const EClientType g_client_type_ = EClientType::win;
#else
#endif

#pragma region namespace
}
#pragma endregion

