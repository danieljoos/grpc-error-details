#include <iostream>
#include <memory>

#include <grpc++/grpc++.h>
#include <grpc++/support/error_details.h>
#include <grpc/grpc.h>

#include "example.grpc.pb.h"
#include "example.pb.h"

// This is the compiled protobuf header of `google::rpc::Status`.
#include "status.pb.h"

class ExampleHelloClient {
public:
    explicit ExampleHelloClient(std::shared_ptr<grpc::Channel> channel)
        : m_stub(example::ExampleHello::NewStub(channel)) {}

    void SaySomething() {
        // Prepare the request
        example::SaySomethingRequest request;
        request.set_phrase("foo");

        // Send the request
        example::SaySomethingResponse response;
        grpc::ClientContext context;
        grpc::Status result = m_stub->SaySomething(&context, request, &response);

        // Extract the error details from the trailers into the `status` object.
        // Then iterate through the repeated `any` field and look for the first
        // value that can be unpacked into our custom `ErrorDetails` type.
        google::rpc::Status status;
        if (grpc::ExtractErrorDetails(result, &status).ok()) {
            for (const auto& entry : status.details()) {
                example::ErrorDetails details;
                if (entry.UnpackTo(&details)) {
                    std::cout << "error-details: " << details.why() << std::endl;
                    break;
                }
            }
        }
    }

private:
    std::unique_ptr<example::ExampleHello::Stub> m_stub;
};

int main(int argc, char** argv) {
    ExampleHelloClient client(
        grpc::CreateChannel("127.0.0.1:50001", grpc::InsecureChannelCredentials()));
    client.SaySomething();
    return 0;
}
