using System;
using System.Linq;
using Grpc.Core;

namespace Example
{
    public static class RpcExceptionExtensions
    {
        public static Google.Rpc.Status GoogleRpcStatus(this RpcException exception)
        {
            return exception.Trailers
                .Where(x => x.Key.Equals("grpc-status-details-bin"))
                .Select(x => Google.Rpc.Status.Parser.ParseFrom(x.ValueBytes))
                .DefaultIfEmpty(null)
                .First();
        }

        public static T ExtractErrorDetails<T>(this RpcException exception)
            where T : Google.Protobuf.IMessage, new()
        {
            T result = default(T);
            exception
                .GoogleRpcStatus()?.Details
                .FirstOrDefault()
                .TryUnpack<T>(out result);
            return result;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            var channel = new Channel("127.0.0.1:50001", ChannelCredentials.Insecure);
            var client = new ExampleHello.ExampleHelloClient(channel);
            try
            {
                var response = client.SaySomething(new SaySomethingRequest { Phrase = "Foo" });
                Console.WriteLine($"ok: { response.Echo }");
            }
            catch (RpcException ex)
            {
                var details = ex.ExtractErrorDetails<ErrorDetails>();
                if (details != null)
                {
                    Console.WriteLine($"error-details: { details.Why }");
                }
            }
            catch (Exception)
            {
            }
            channel.ShutdownAsync().Wait();
        }
    }
}
