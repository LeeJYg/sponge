Assignment 5 Writeup
=============

My name: Lee JooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about [34] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
[필요한 데이터는 세 가지였다. 1. ip 주소와 ethernet address를 mapping해서 가지고 있는 _addr_table, 2. ARP 요청들에 대해 보내고 나서 얼마나 시간이 지났느지 확인하는 _ARP_status(-1: 응답 받지 못함. 0: 응답 받음, 양수: 응답 받고 지난 시간, 음수: 응답 받지 못하고 지난 시간.), 3, 보내지기를 기다리고 있는 data gram들을 모아 놓는 _waiting_dgram. 이에 더해 _waiting_dgram을 필요할 때마다 확인해서 보내진 dgram들을 erase하는 함수 waiting_dgram_check가 추가 함수로 사용되었다.

send_datagram 함수는 TCPConncetion이나 라우터가 다음 hop으로 나갈 dgram을 전송하려고 할 때 호출된다. 이 때 이더넷 주소가 이미 알려져 있다면 즉시 전송하고, 아니라면 ARP REQUEST를 브로드케스트하고 시간을 측정한다. 이 때 시간을 보고 마지막으로 요청이 보내지진 5초가 되기 전엔 다시 보내지 않도록 하였다. 이 시간 관리는 tick 함수에서 처리하였기에 send_datagram 함수에서는 이더넷 프레임을 만들어 _frame_out에 push 하는 것까지만 진행하였다. push하기 위한 이더넷 프레임 ef는 인자로 들어온 next_hop의 IPv4 주소가 _addr_table에 있을 경우, 즉, 보낼 이더넷 주소를 이미 알고 있는 경우 _addr_table을 확인해 해당 주소로 ef를 초기화 한다. 만약 그렇지 않다면, _ARP_status를 확인해 ARP에 대한 OPCODE_REQUEST를 보내게 한다. 이 데이터들은 ef의 payload로 저장되고, 나머지 header 부분을 작성한 다음 _ARP_status와 _waiting_dgram을 업데이트한다. 아직은 ARP 요청에 대해 응답을 받지 못했으므로 _ARP_status는 -1이다.

recv_dgram은 이더넷 프레임 ef를 받고 IPv4에 대한 프레임인지, 아니면 ARP 요청인지 판단한 다음 parse 함수를 불러 data를 꺼내오도록 하는 함수다. 만약 IPv4에 대한 프레임이라면 parse 함수를 이용해 InternetDatagram 타입의 결과를 얻어 반환하고, ARP 요청이라면 요청에 대한 응답을 받은 것이니 _ARP_status를 0으로 설정하고, 만약 타입이 요청이라면 응답에 대한 프레임을 만든 후 해당 이더넷 프레임을 _frame_out에 push 하도록 한다. 이후 _ARP_status를 waiting_dgram_check 함수를 불러 업데이트 한다.

tick 함수는 _ARP_status를 탐색하면서 _ARP_status가 양수라면 응답을 받은 후부터 지난 시간을 측정하기 위해 ms_since_last_tick를 더해주고, 음수라면 응답 받지 못한 상태로부터 지난 시간을 측정하기 위해 ms_since_last_tick를 빼주도록 한다. 이때 _ARP_status를 지우지 않고 그대로 가지고 있으면 언젠어 overflow 문제가 일어날 수 있기 때문에 특정 시간이 지난 이후부턴 _ARP_status를 빼주도록 한다. 이 때 음수 부분을 보면 5000 ms = 5 sec가 지났을 때 erase하고 양수 부분에서는 30 sec가 지나면 erase하게 만들었다.

waiting_dgram_check 함수는 _waiting_dgram을 탐색하면서 _ARP_status에 있으면서 value가 양수인 dgram(= 응답 받았는데 아직도 _ARP_status에 있는 dgram)을 빼주도록 한다.]

Implementation Challenges:
[assn4를 하고 난 뒤라 그런가 상대적으로 이전 같은 부담감은 없었다. 다만 어려운 점이 아예 없었던 것은 아닌었는데, 우선 private member로 무엇을 어떤 타입으로 설정할 것인지가 문제였다. IPv4와 이더넷 주소를 매핑하는 테이블이 필요하다는 것은 비교적 직관적이었지만 ARP 요청을 어떤 식으로 처리하고, 그 시간을 어떤 방식으로 저장할지가 관건이었다. 처음에는 ARP_status에 시간을 측정하는 변수를 하나 더 집어넣으려고 했는데, tuple에 3개나 되는 element가 생기니 recv_dgram과 send_dgram이 처리하는 게 불편해졌다. 그래서 시간을 관리하기 위해 waiting_dgram을 새롭게 선언하였다. 또한 시간 관리를 하는 것에 있어 '요청을 한 이후부터의 시간'과 '요청에 대한 응답을 받은 이후부터의 시간'이 다르게 처리되어야 한다는 것을 깨닫고, 이를 각각 음수와 양수로 나눠 처리하면 되겠다고 생각했다. 이렇게 변수와 데이터 관리 방법을 생각해내는 것이 첫 번째 Challenge였다.

물론 이러한 데이터 처리 과정이 비교적 직관적이지가 않아 정작 두 함수를 작성할 때 어려운 점이 있기도 했다. 하지만 제대로된 이해가 뒷받침되고 나서부턴 그리 큰 문제가 아니었고, recv_dgram과 send_dgram의 함수에서 조건으로 처리해야 할 부분을 작성할 수 있었다. 이때 이 조건을 설정하는 것도 Challenge의 일종이었는데, 이더넷 주소를 이미 알고 있는 경우를 어떻게 표현해야 할지, 그 이후에는 else if로 해야 할지 그냥 if로 해야 할지 등이 나름의 고민이었다. 또 이더넷 프레임을 만들어 frame_out에 push할 때, 어떻게 이더넷 프레임을 만들어야 하는지 몰랐다. 이를 위해 새롭게 추가된 tcp_helpers의 내용을 읽고 이해하는 것 역시 문제였다. 이더넷프레임 타입의 변수에는 무엇이 있는지, 어떻게 초기화해야하는지, 초기화에 필요한 변수가 무엇인지 파악하는 것이 가장 큰 Challenge 중 하나였다.]

Remaining Bugs:
[테스트는 모두 통과했다.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
