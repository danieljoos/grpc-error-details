
#include <grpc++/grpc++.h>
#include <grpc++/support/error_details.h>
#include <grpc/grpc.h>

#include "example.grpc.pb.h"
#include "example.pb.h"

// This is the compiled protobuf header of `google::rpc::Status`.
#include "status.pb.h"

class ExampleHelloImpl final : public example::ExampleHello::Service {
  grpc::Status SaySomething(
      grpc::ServerContext* context, const example::SaySomethingRequest* request,
      example::SaySomethingResponse* response) final override {
    // Create a protobuf message object that represents our custom information
    example::ErrorDetails details;
    details.set_why("some detailed error information");

    // Create a google::rpc::Status protobuf message object that will be sent in
    // the grpc trailers. The custom error-details object (see above) will be
    // packed into an `Any` field.
    google::rpc::Status status;
    status.set_code(grpc::StatusCode::UNKNOWN);
    status.set_message("normal error message");
    status.add_details()->PackFrom(details);

    // Call the `SetErrorDetails` support function of grpc.
    grpc::Status result;
    grpc::SetErrorDetails(status, &result);
    return result;
  }
};

int main(int argc, char** argv) {
  ExampleHelloImpl service;
  grpc::ServerBuilder builder;
  builder.AddListeningPort("127.0.0.1:50001",
                           grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  builder.BuildAndStart()->Wait();
  return 0;
}
