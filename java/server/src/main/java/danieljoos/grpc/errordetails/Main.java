package danieljoos.grpc.errordetails;

import com.google.protobuf.Any;
import com.google.rpc.Status;
import example.Example;
import example.ExampleHelloGrpc;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.protobuf.StatusProto;
import io.grpc.stub.StreamObserver;

import java.io.IOException;

public class Main {

    private static class ExampleHelloImpl extends ExampleHelloGrpc.ExampleHelloImplBase {
        @Override
        public void saySomething(Example.SaySomethingRequest request, StreamObserver<Example.SaySomethingResponse> responseObserver) {
            // Build the custom error details protobuf object.
            Example.ErrorDetails errorDetails = Example.ErrorDetails.newBuilder()
                    .setWhy("my custom error details")
                    .build();
            // Pack it into a com.google.rpc.Status object.
            // This object is transferred in the Trailers section over the wire.
            // Code and message will be copied into the headers automatically.
            Status errorStatus = Status.newBuilder()
                    .setCode(io.grpc.Status.Code.UNKNOWN.value())
                    .setMessage("Something bad happened")
                    .addDetails(Any.pack(errorDetails))
                    .build();
            // Build a StatusException and hand it over to the response observer.
            responseObserver.onError(StatusProto.toStatusException(errorStatus));
        }
    }

    private static class ExampleHelloServer {
        private Server server_;

        private void start() throws IOException {
            server_ = ServerBuilder.forPort(50001)
                    .addService(new ExampleHelloImpl())
                    .build()
                    .start();
            Runtime.getRuntime().addShutdownHook(new Thread(ExampleHelloServer.this::stop));
        }

        private void stop() {
            if (server_ != null) {
                server_.shutdown();
            }
        }

        private void blockUntilShutdown() throws InterruptedException {
            if (server_ != null) {
                server_.awaitTermination();
            }
        }
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        final ExampleHelloServer server = new ExampleHelloServer();
        server.start();
        server.blockUntilShutdown();
    }
}
