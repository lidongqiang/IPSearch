#ifndef C_SOCKET_H
#define C_SOCKET_H
#include <iostream>

/* �׽��ֲ����� */
// ���÷������׽���
//int socket_set_server(unsigned short port, int listen_num);
// ��������
//int socket_accept(int socket_server);
// ����
int socket_write(SOCKET socket_fd, std::string strMsg);
// ��
int socket_read(SOCKET socket_fd, std::string &strMsg);
// �ر��׽���
void socket_close(SOCKET socket_fd);
// ���ӷ�����
//int socket_connect(const char * ip, unsigned short port);



#endif
