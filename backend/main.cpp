/*
 * Backend
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "generated/machinevision.grpc.pb.h"
#include "generated/elettronica.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using machinevision::AnalisiFrutta;
using machinevision::DatiFrutto;
using machinevision::Stato;

using elettronica::Allarme;
using elettronica::Error;
using elettronica::Stop;

using elettronica::SelezionaFrutto;
using elettronica::Posizion;
using elettronica::Scarto;

// Logic and data behind the server's behavior.
class AnalisFruttaServiceImpl final : public AnalisiFrutta::Service {
	Status setFruttoAnalizzato(ServerContext* context, const DatiFrutto* request, Stato* reply) override {

		if (request->qualita() < 0)  {
			/* il frutto è da scartare
			   imposta variabile elettronica::Scarto a true
			*/
		}

		if (0) // controlla stato errore backend
			reply->set_stato(false);
		else
			reply->set_stato(true);

		return Status::OK;
	}
};

class AllarmeServiceImpl final : public Allarme::Service {
	Status setError(ServerContext* context, const Error* request, Stop* reply) override {

		/*
		 * l'elettronica ha ritornato un errore
		 * prendere decisione se fermare il processamento
		 */
		if (request->errore() < 0)
			reply->set_ferma(false);
		else
			reply->set_ferma(true); //ferma processamento
		return Status::OK;
	}
};

class SelezionaFruttoServiceImpl final : public SelezionaFrutto::Service {
	Status getAnalisi(ServerContext* context, const Posizion* request, Scarto* reply) override {
		if (request->posizione() == 0) {
			/*
			 * l'elettronica sta processando il frutto dell'ultima analisi
			 */

			if (0) // controlla variabile impostata da metodo AnalisiFrutta::Service::setFruttoAnalizzato
				reply->set_scarta(false);
			else
				reply->set_scarta(true); //scarta frutto corrente
		}
		return Status::OK;
	}
};

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	AnalisFruttaServiceImpl service0;
	AllarmeServiceImpl service1;
	SelezionaFruttoServiceImpl service2;

	ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service0);
	builder.RegisterService(&service1);
	builder.RegisterService(&service2);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main(int argc, char** argv) {
	RunServer();

	return 0;
}
