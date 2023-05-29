Assignment 7 Writeup
=============

My name: Lee JooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

My assignment partner's name: 이정현

My assignment partner's POVIS ID: dlwjdgus55

My assignment partner's ID (numeric): 20200806

This assignment took me about [7] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Solo portion:
[혼자서 VM copy를 만들어 서로 conversation을 하게 해보았다. 그 결과, 서로 오류 없이 정상 작동 하는 것을 확인할 수 있었다. server와 client를 연결한 상태에서 server에서 "hello"를 보냈을 때, client의 터미널에서도 "hello"가 뜨는 것을 볼 수 있었다. debug를 통해 확인해본 결과, server의 기준으로 listening하는 중에 connecting이 형성되면 가장 먼저 client가 보낸 request가 internet을 통해 들어온다. 이게 router를 거쳐 server의 host로 들어오면 host->router 방향으로 reply를 보내게 되고 client는 자신의 router로부터 해당 reply를 받음으로써 "Successfully conected to ..." 메세지가 나오게 된다. 이후 "hello"라는 메시지를 server에서 보내게 되면 서버는 host->router->internet을 거쳐 메세지를 보내고 그 역순으로 ack을 받게 된다. client는 host에서 router로 ack을 보내게 된다. 이러한 과정이 정상적으로 동작하는 것을 볼 수 있었다.

하지만 one-megabyte file의 transfer은 실패했다. server 쪽에서 "dd if=/dev/urandom bs=1M count=1 of=/tmp/big.txt" command를 이용해 1M txt를 만들고, 이를 "./apps/lab7 server tomahawk.postech.ac.kr 63946 < /tmp/big.txt "으로 listening 시켜놓은 다음 client vm에서 "</dev/null ./apps/lab7 client tomahawk.postech.ac.kr 63947 > /tmp/big-received.txt"를 통해 connection을 시도했다. 그 결과 실제로 client 쪽 vm에서 big-received.txt 파일을 받았고, /tmp 경로에서 해당 파일을 확인할 수 있었다. 하지만 server 쪽에서 터미널에 나온 결과는 "Thread ending from exception: IPv4Datagram::serialize: payload is wrong size"였다. lab7 코드를 분석해본 결과, 해당 코드는 tcp_helper에 있는 ipv4_datagram.cc 파일에서 runtime error를 throw했을 때 발생하는 문제로, payload의 size와 header의 payload length가 다를 때 생기는 문제였다. 이를 해결하기 위해 tcp_connection 파일과 tcp_sender 파일을 보았다. connection에 문제가 있거나, sender가 segment를 보낼 때 header에 업데이트를 제대로 하지 않았을 것이란 생각 때문이었다. 하지만 해당 부분을 고쳐도 동일한 문제가 지속적으로 발견되었다.]

Group portion:
[팀 이름은 Robert Khan, 파트너는 이정현(dlwjdgus55) 학우였다.

안타깝게도 해당 학우 분의 코드와 tcp connection을 생성하는 것은 성공하지 못했다. 1M file을 보내는 것을 성공하지 못한 건 현재 내 코드의 문제가 있기 때문이라고 할 수 있겠지만, conversation이 되지 않은 것은 학우 분의 코드에 이상이 생긴 것이 원인으로 생각된다. conversation을 연결하려던 때에 정현 학우 분의 코드는 server, client의 역할을 제대로 수행하지 못하는 상태였다. 그래서 self connection에도 문제가 있어 연결이 되지 못했었다. 혹시 server나 client, 둘 중 하나로는 정상 작동하는데 다른 쪽에서 문제가 있는 것 아닐까 하여 server, client를 번갈아 하면서 connection을 시도해보았다. 하지만 시간이 지나도 successfully connected 메세지는 나타나지 않았다.]

Creative portion (optional):
[]

Other remarks:
[현재 lab7 파일은 단일 thread와의 연결만 서비스하고 있다. 그 부분에서 착안하여 다중 스레드 연결을 시도해보려고 했지만 txt 파일의 연결 실패로 해당 부분의 구현에 실패했다.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
