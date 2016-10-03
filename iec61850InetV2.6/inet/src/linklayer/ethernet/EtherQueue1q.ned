//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.linklayer.ethernet;
import inet.linklayer.IOutputQueue;
import inet.linklayer.queue.DropTailQueue;
import inet.linklayer.queue.PriorityScheduler;
import inet.linklayer.queue.PrioritySchedulerEx;


//
// TODO documentation
//
module EtherQueue1q like IOutputQueue
{
    @display("bgb=464,506;i=block/queue");
    gates:
        input in;
        output out;

    submodules:
        data: DropTailQueue {
            @display("p=237,470");
        }
        classifier: Ether1qClassifier {
            @display("p=68,264");
        }
        queue1: DropTailQueue {
            @display("p=237,410");
        }

        queue2: DropTailQueue {
            @display("p=237,348");
        }
       
        queue3: DropTailQueue {
            @display("p=237,286");
        }

        queue4: DropTailQueue {
            @display("p=237,225");
        }

        queue5: DropTailQueue {
            @display("p=237,165");
        }

        queue6: DropTailQueue {
            @display("p=237,104");
        }
        
        queue7: DropTailQueue {
            @display("p=237,43");
        }

        scheduler: PriorityScheduler {
            @display("p=364,240");
        }
    connections:
        //for i=0..sizeof(in)-1 {
        //in++ --> classifier.In++;
        in --> classifier.In++;
            //in --> classifier.In++;
        //}       
        classifier.Out++ --> queue7.in;
        classifier.Out++ --> queue6.in;
        classifier.Out++ --> queue5.in;
        classifier.Out++ --> queue4.in;
        classifier.Out++ --> queue3.in;
        classifier.Out++ --> queue2.in;
        classifier.Out++ --> queue1.in;
        classifier.defaultOut --> data.in;
        queue7.out --> scheduler.in++;
        queue6.out --> scheduler.in++;
        queue5.out --> scheduler.in++;
        queue4.out --> scheduler.in++;
        queue3.out --> scheduler.in++;
        queue2.out --> scheduler.in++;
        queue1.out --> scheduler.in++;
        data.out --> scheduler.in++;
        scheduler.out --> out;
}

