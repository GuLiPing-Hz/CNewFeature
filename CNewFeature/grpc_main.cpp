#include <sstream>
#include <windows.h>

#ifdef min
#undef min
#endif

#include <grpcpp/grpcpp.h>
#include "grpc-example/rpcfirst.grpc.pb.h"
class RPCService :public RPCFirst::Demo::Service
{
public:
	RPCService() {}
	virtual ~RPCService() {}

	// RPC普通方法，一次调用一次返回
	virtual ::grpc::Status SayHello(::grpc::ServerContext* context, const ::RPCFirst::ReqHello* request, ::RPCFirst::RespHello* response) {
		printf("context=%p\n", context);

		std::stringstream ss;
		ss << "[c++] Hi " << request->name() << ", you are " << request->age();
		response->set_hi(ss.str());
		return ::grpc::Status::OK;
	}
};

void TestRPCClient() {
	std::string server_address("0.0.0.0:8001");
	RPCService service;

	::grpc::ServerBuilder builder;
	builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
}

#pragma comment(lib,"ws2_32.lib")

void main() {
	//gRPC demo
	TestRPCClient();
}
