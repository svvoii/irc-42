#include "ServerManager.hpp"

ServerManager::ServerManager() {

	FD_ZERO(&_recv_fd_pool);
	FD_ZERO(&_send_fd_pool);
	_max_fd = 0;

	std::cout << MAGENTA << "\tServersManager default constructor called" << RESET << std::endl;

	// DEBUG PRINT SERVERS DATA
	_server.printServerData();

	// This will start the main loop of the server
	run();
}

ServerManager::~ServerManager() {

	std::cout << RED << "\tServersManager destructor called" << RESET << std::endl;
}

void	ServerManager::_fcntl() {

	int	fcntl_ret = 0;
	int	serverFd = _server.getServerFd();

	fcntl_ret = fcntl(serverFd, F_SETFL, O_NONBLOCK);
	checkErrorAndExit(fcntl_ret, "fcntl() failed. Exiting..");

	_addToSet(serverFd, &_recv_fd_pool);

	_max_fd = serverFd; // isn't this handled in addToSet?
}

void	ServerManager::_addToSet(int fd, fd_set *set) {

	if (fd > _max_fd) {
		_max_fd = fd;
	}
	FD_SET(fd, set);
}

void	ServerManager::_removeFromSet(int fd, fd_set *set) {

	if (fd == _max_fd) {
		_max_fd--;
	}
	FD_CLR(fd, set);
}

void	ServerManager::_closeConnection(int fd) {
	std::cout << timeStamp() << YELLOW << "[!] Closing connection with fd:[" << fd << "]." << RESET << std::endl;

	if (FD_ISSET(fd, &_recv_fd_pool)) {
		_removeFromSet(fd, &_recv_fd_pool);
	}
	if (FD_ISSET(fd, &_send_fd_pool)) {
		_removeFromSet(fd, &_send_fd_pool);
	}
	close(fd);
	clientsMap.erase(fd);
}

std::string	ServerManager::timeStamp() {

	std::time_t currentTime = std::time(NULL);
    std::tm* now = std::localtime(&currentTime);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M:%S]", now);

	return std::string(buffer);
}

void	ServerManager::checkErrorAndExit(int returnValue, const std::string& msg) {

	if (returnValue == -1) {

		std::cerr << RED << "\t[-]" << msg << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	ServerManager::run() {

	fd_set	recv_fd_pool_copy;
	fd_set	send_fd_pool_copy;
	int		select_ret = 0;

	// This will set the server socket fd to non-blocking mode and add it to the recv_fd_pool
	_fcntl();

	while (true) {

		recv_fd_pool_copy = _recv_fd_pool;

		select_ret = select(_max_fd + 1, &recv_fd_pool_copy, NULL, NULL, NULL);
		checkErrorAndExit(select_ret, "select() failed. Exiting..");

		for (int fd = 3; fd <= _max_fd; fd++) {

			if (fd == _server.getServerFd()) {

				_accept();
			}
			else if (FD_ISSET(fd, &recv_fd_pool_copy)) {

				_handle(fd);
			}
			else if (FD_ISSET(fd, &send_fd_pool_copy)) {
				_respond(fd);
			}
		}

		// check for timeout ?!
	}
}

void	ServerManager::_accept() {

	struct sockaddr_in	address;
	socklen_t			address_len = sizeof(address);
	int					serverFd = _server.getServerFd();
	int					return_value = 0;
	int					clientFd = 0;

	clientFd = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&address_len);
	if (clientFd == -1) {
		std::cerr << RED << "\t[-] Error accepting connection.. accept() failed..";
		std::cerr << " serverFd: [" << serverFd << "], clientFd:[" << clientFd << "]" << std::endl;
		std::cerr << RESET << std::endl;
		return ;
	}

	std::cout << timeStamp() << GREEN << "[+] New connection to [" << _server.getServerName() << "] fd:[" << serverFd << "], client fd:[" << clientFd << "], IP:[" << inet_ntoa(address.sin_addr) << "]" << RESET << std::endl;

	_addToSet(clientFd, &_recv_fd_pool);

	// This will set the client socket fd to non-blocking mode (needed for select(), read(), recv(), write(), send()..)
	return_value = fcntl(clientFd, F_SETFL, O_NONBLOCK);

	if (return_value == -1) {
		std::cerr << RED << "\t[-] Error setting socket to non-blocking mode.. fcntl() failed." << RESET << std::endl;
		_closeConnection(clientFd);
		return ;
	}

	int	enable = 1;
	return_value = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	checkErrorAndExit(return_value, "\t[-] Error setting socket options.. setsockopt() failed.");

	// Add the new client to the clientsMap
	addClient(clientFd, address);

	log(clientFd);
}

/*
** This is handling the requests from the irc client side
*/
void	ServerManager::_handle(int fd) {

	char	buffer[BUF_SIZE] = {0};
	int		bytes_read = 0;

	bytes_read = read(fd, buffer, BUF_SIZE);
	std::cout << timeStamp();

	if (bytes_read == 0) {
		std::cout << YELLOW << "[!] bytes_read == 0 from client fd:[" << fd << "]" << RESET << std::endl;
		//std::cout << YELLOW << "[!] Connection closed by the client. ";
		_closeConnection(fd);
		return ;
	}
	else if (bytes_read < 0) {
		std::cerr << RED << "[-] Error reading data from the client.. read() failed." << RESET << std::endl;
		_closeConnection(fd);
		return ;
	}

	// clientsMap[fd].requestBuffer.append(buffer, bytes_read);
	clientsMap[fd].clientMessageBuffer = std::string(buffer, bytes_read);

	std::cout << CYAN << "[*] received from client fd[" << fd << "]: " << RESET;
	std::cout << clientsMap[fd].clientMessageBuffer;

	_removeFromSet(fd, &_recv_fd_pool);
	_addToSet(fd, &_send_fd_pool);
}

/*
** The following function is handling the responses logic from server to the irc client
*/
void	ServerManager::_respond(int fd) {

	int		bytes_sent = 0;
	int		bytes_to_send = clientsMap[fd].responseBuffer.length();

	bytes_sent = send(fd, clientsMap[fd].responseBuffer.c_str(), bytes_to_send, 0);

	std::cout << timeStamp();

	if (bytes_sent == -1) {
		std::cerr << RED << "[-] Error sending data to the client.. send() failed." << RESET << std::endl;
		_closeConnection(fd);
		return ;
	}
	else {
		std::cout << GREEN << "[+] Response sent to client fd:[" << fd << "]" << RESET << std::endl;
	}

	_removeFromSet(fd, &_send_fd_pool);
	_addToSet(fd, &_recv_fd_pool);

	clientsMap[fd].clientMessageBuffer.clear();
	clientsMap[fd].responseBuffer.clear();
}

void	ServerManager::addClient(int clientFd, struct sockaddr_in &address) {

	t_client	buff;

	char	*client_ip = inet_ntoa(address.sin_addr);
	if (client_ip == NULL) {
		std::cerr << RED << "\t[-] Error: inet_ntoa() failed." << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	buff.port = address.sin_port;
	buff.hostName = client_ip;
	buff.nickName = "..default-test-NICK-NAME";
	buff.userName = "..default-test-USER-NAME";
	buff.realName = "..default-test-REAL-NAME";
	buff.clientMessageBuffer = "..default-test-REQUEST";
	buff.responseBuffer = "..default-test-RESPONSE";

	clientsMap.insert(std::make_pair(clientFd, buff));
}

void	ServerManager::log(int clientFd) {

	t_client &client = clientsMap[clientFd];

	std::cout << timeStamp() << YELLOW << "[!] Logging client fd:[" << clientFd << "]" << RESET << std::endl;
	std::cout << YELLOW << "\tport: " << client.port << RESET << std::endl;
	std::cout << YELLOW << "\thostName: " << client.hostName << RESET << std::endl;
	std::cout << YELLOW << "\tnickName: " << client.nickName << RESET << std::endl;
	std::cout << YELLOW << "\tuserName: " << client.userName << RESET << std::endl;
	std::cout << YELLOW << "\trealName: " << client.realName << RESET << std::endl;
	std::cout << YELLOW << "\trequestBuffer: " << client.clientMessageBuffer << RESET << std::endl;
	std::cout << YELLOW << "\tresponseBuffer: " << client.responseBuffer << RESET << std::endl;

}
