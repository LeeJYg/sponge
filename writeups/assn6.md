Assignment 6 Writeup
=============

My name: Lee JooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about [19] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
[이번 과제에서 가장 중요한 부분은 longest prefix를 찾고 이를 rounting에 접목시키는 것이다. 이를 위해 다음과 같은 rounte element를 private member로 선언하였다.
_interface_num: route에 대한 인터페이스 번호. _pre_length: route의 prefix 길이. _prefix: route의 prefix. _next_hop_addr: 다음 홉의 optional ip 주소.
이들을 하나의 구조체로 묶은 후, 이를 vector로 선언하여 routing시 필요한 정보들을 저장할 수 있도록 하였다.

match_longest_prefix 함수는 주어진 addr와 가장 긴 prefix의 일치를 찾는 기능을 하는 함수이다. 위에서 선언한 route info를 가지고 있는 _routes 벡터를 탐색하면서 저장된 각 RouteElement를 보고, 주어진 주소와 일치하는 route를 찾는다. 이때 mask를 이용하여 prefix 부분만 볼 수 있도록 하고, 이 때 prefix가 일치하는 주소들 중 가장 긴 prefix length를 가지고 있는 route element의 idx를 반환하게 한다. 여기서 반환된 idx는 후 route_one_datagram에서 사용된다.

route_one_datagram에서는 하나의 datagram을 routing할 수 있도록 한다. 우선 routing을 할 element의 idx를 match_longest_prefix 함수를 통해 알아내고, 인자로 들어온 datagram이 살아있는지 확인한다. ttl을 확인하여 1보다 큰 경우 idx를 이용해 처리하는데, routes에 next_hop 주소가 존재하는 경우 그 값을 datagram과 함께 보낸다. 만약 있지 않다면 datagram에 있는 dst ip addr를 함께 동봉하여 보낸다.]

Implementation Challenges:
[가장 기본적으로 longest_prefix를 찾는 것이 문제였다. 라우터들마다 일종의 routing table이 있다고 배웠기에 vector와 struct를 이용하여 routes를 만들어야겠단 생각을 할 순 있었는데, 그 중에서 무엇을 어떻게 찾아야 하는지가 문제였다. 고민하다 보니 컴퓨터 구조 시간에 배웠던 mask 개념이 떠올랐고, 그것을 이용하면 prefix 부분만 효율적으로 탐색해볼 수 있겠단 생각을 할 수 있었다. 그렇게 하여 mask를 선언해 addr와 route의 prefix를 비교할 수 있었고, match_longest_prefix 함수를 구현할 수 있었다. 그 외에 dgram을 어떻게 보내야 했는지 막막했던 부분은 description을 자세히 읽어보니 interface 함수를 사용하란 내용이 있어 해결할 수 있었다.

또한 route_one_datagram에서 ttl 값을 확인해야 한다는 것을 어느 시점에 해야 할 지 선택해야 했다. 처음에는 idx에 해당하는 route를 찾고 나서 ttl을 -1 했었는데, 그 결과 test에서 fail이 발생했었다. ttl은 routing에 관계 없이 흘러가야 한다는 것을 망각하고 코드를 작성했던 것 같다. 또한 route elements의 구조체를 작성할 때 내부 member를 무엇으로 해야 하는지 생각했었다. route 하나를 나타내기 위해 필요한 값들이 무엇일까 생각해보다가 마침 add_route 함수에 들어오는 인자를 통해 필요한 info를 얻을 수 있었다.]

Remaining Bugs:
[테스트는 모두 통과했다.]

- Optional: I had unexpected difficulty with: [내 assn4 bundle이 제대로 제출되지 않았다는 것 때문에 정신적인 데미지가 상당히 컸다. 가장 unexpected한 difficulty였다. 분명 제대로 체크해서 160개 test가 다 pass하는 걸 보고 제출했는데.]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
