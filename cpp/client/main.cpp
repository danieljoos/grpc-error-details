#include <iostream>
#include <memory>
#include <optional>

#include <grpc++/grpc++.h>
#include <grpc++/support/error_details.h>
#include <grpc/grpc.h>

#include "example.grpc.pb.h"
#include "example.pb.h"

// This is the compiled protobuf header of `google::rpc::Status`.
#include "status.pb.h"

/** 
 * Extract the error details from the trailers into the `status` object.
 * Then iterate through the repeated `any` field `details` and look for the first
 * value that can be unpacked into our custom `ErrorDetails` type.
 */
std::optional<example::ErrorDetails> GetErrorDetails(const grpc::Status& status) {
    google::rpc::Status trailerStatus;
    if (grpc::ExtractErrorDetails(status, &trailerStatus).ok()) {
        example::ErrorDetails result;
        for (const auto& entry : trailerStatus.details()) {
            if (entry.UnpackTo(&result)) {
                return result;
            }
        }
    }
    return std::nullopt;
}

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
        grpc::Status status = m_stub->SaySomething(&context, request, &response);

        // Error handling
        if (status.ok()) {
            std::cout << "ok: " << response.echo() << std::endl;
        } else if (auto details = GetErrorDetails(status)) {
            std::cout << "error-details: " << details->why() << std::endl;
        } else {
            std::cerr << "error: " << status.error_message() << std::endl;
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
