using System;
using System.Threading;
using System.Threading.Tasks;
using Grpc.Core;
using Google.Protobuf;
using Google.Protobuf.WellKnownTypes;

namespace Example
{
    public static class GoogleRpcStatusExtensions
    {
        public static Status ToGrpcStatus(this Google.Rpc.Status status)
            => new Status((StatusCode)status.Code, status.Message);

        public static RpcException ToRpcException(this Google.Rpc.Status status)
        {
            var trailers = new Metadata();
            trailers.Add("grpc-status-details-bin", status.ToByteArray());
            return new RpcException(status.ToGrpcStatus(), trailers);
        }

        public static Google.Rpc.Status AddDetails(this Google.Rpc.Status status, IMessage message)
        {
            status.Details.Add(Any.Pack(message));
            return status;
        }
    }

    class ExampleHelloImpl : ExampleHello.ExampleHelloBase
    {
        private static RpcException RpcExceptionWithDetails(string message, IMessage details)
            => new Google.Rpc.Status { Code = (int)StatusCode.Unknown, Message = message }
                .AddDetails(details).ToRpcException();

        public override Task<SaySomethingResponse> SaySomething(
            SaySomethingRequest request, ServerCallContext context)
        {
            // Our custom error details protobuf message
            var errorDetails = new ErrorDetails()
            {
                Why = "Detailed text about why the error happened!"
            };

            throw RpcExceptionWithDetails("My normal error message", errorDetails);
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Server server = new Server
            {
                Services = { ExampleHello.BindService(new ExampleHelloImpl()) },
                Ports = { new ServerPort("127.0.0.1", 50001, ServerCredentials.Insecure) }
            };
            server.Start();

            var exitEvt = new AutoResetEvent(false);
            Console.CancelKeyPress += (sender, eventArgs) =>
            {
                eventArgs.Cancel = true;
                exitEvt.Set();
            };
            exitEvt.WaitOne();

            server.ShutdownAsync().Wait();
        }
    }
}
