// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using System;
using Grpc.Core;
using Elettronica;

namespace controlloelettronica
{
    class Program
    {
        public static void Main(string[] args)
        {
            Channel channel = new Channel("127.0.0.1:50051", ChannelCredentials.Insecure);

            var clientAlarm = new Allarme.AllarmeClient(channel);
            var clientSelection = new SelezionaFrutto.SelezionaFruttoClient(channel);
            
            for (;;) {
                /* Legge lo stato di errore dell'elettronica */
                Error request0 = new Error { Errore = 0 };
                var reply = clientAlarm.setError(request0);
                /* risposta: se il processamento deve fermarsi o no */
                if (reply.ferma) {
                    Console.WriteLine("Stopping fruit processing");
                    break;
                }
                
                /* Legge la coda di processamento dei frutti */
                Posizion request1 = new Posizion { Posizione = 0 };
                var reply = clientSelection.getAnalisi(request1);
                /* risposta: se il frutto processato è da scartare o no */
                if (reply.scarta)
                    Console.WriteLine("Skipping current fruit");
            }

            channel.ShutdownAsync().Wait();
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }
    }
}
