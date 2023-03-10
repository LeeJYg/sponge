Assignment 1 Writeup
=============

My name: Lee JooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about 16 hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
[우선 substring이 들어오면 그게 capacity 내 어디 위치할 지를 알아야 한다. 따라서 특정 index에 값이 들어왔는지 아닌지를 판단할 수 있는 data structure가 필요하고, 이 크기는 capacity를 넘지 않아야 한다.
    - 이 경우, string, set 등 다양한 선택지가 있을 수 있었지만, 중요한 것은 특정 index에 값이 들어왔는지를 따지는 것이다. 따라서 insert, erase, find 같은 함수가 많이 호출되므로 이 함수의 시간 복잡도가 O(1)인 unordered_set을 선택했다.

unassembled data가 얼마나 들어왔는지를 판단해야 unassembled_bytes()와 empty()를 구현할 수 있으므로 size_t unassembled_data를 선언했다.

substring으로 들어온 input을 임시로 저장할 곳이 필요하고, 이 크기는 물론 capacity를 넘을 필요가 없다.(그걸 넘는 write의 경우는 자동으로 discarded되므로) 따라서 capacity 크기만큼의 string인 Data_NotStored를 선언했다.

어느 위치에서부터 unassembled_data가 시작되는지를 알 수 있게 해당 위치의 index를 가리키는 size_t first_unassembled_index를 선언했다.

입력이 끝이 났는지를 판단하기 위해 bool is_end를 선언했다. 초기화 값은 물론 false로 해야 입력이 끝날 때 true로 바꿀 수 있다.]

Implementation Challenges:
[우선 index를 어떻게 관리해야 하는지 고르는 것이 가장 어려웠다. string으로 하자니 탐색하는 것이 막막하고, 탐색의 필요성을 생각하니 set, multiset 등 다양한 선택지가 떠올라 막막했다. 결국 선택 기준을 시간 복잡도로 잡으면 되겠단 생각을 했고, 찾아본 결과 가장 효율적인 것이 unordered set이었다. 애초에 index는 있는지만 확인하면 되니 정렬도 필요 없었고, 따라서 이걸 선택해 코드를 작성했다.

또한 당연하게도, push_substring을 구현하는 것이 난해하고 어려웠다. 우선 substring을 받아들일 수 있는 조건이 무엇인지부터 파악해야 했는데, index값이 out of capacity인 경우 거절해야 한다. 이 과정에서 substring의 last index와 capacity의 last index를 자주 사용하게 됨을 알 수 있었고, 이를 아예 변수로 선언함으로 난해함을 조금 해소할 수 있었다.

is_end 변수를 언제 true로 만들어야 하는지는 비교적 쉬웠는데, 문제는 실제로 unassembled data를 다루는 곳이었다. data를 어떻게 처리해야 하는지 막막했었고, 결국 들어오는 string을 char 단위로 생각해 for문으로 반복해서 작업해야겠다는 결론에 도달했다. 그렇게 하자 다음 부분부터는 생각한 대로 작성할 수 있었던 것 같다.

추가적으로 코드 작성이 완료된 이후 test 8/16에서 substr 오류가 발생했는데, 이를 해석하는 것에 8시간 이상 걸렸다. gdb를 이용해 오류 원인을 분석해봤고, 이에 더해 cout을 이용하여 내부 변수들이 어떻게 업데이트 되는지 추이를 지속적으로 확인했다. 그 결과 first_unassembled_index가 예상했던 것과 다르게(output에서 read가 occur되면 buffer를 아예 지워버리는데, first_unassembled_index는 여전히 지워지기 전 index를 가리키고 있고, 여기서 substr의 pos가 예상 외의 위치를 지정하느 오류가 발생했다.) 작동하는 것이 확인되었고, 결국 Data_NotStored 자체를 지속적으로 resize하는 과정을 통해 이 문제를 해결할 수 있었다.]

Remaining Bugs:
[남아 있는 버그는 발견되지 않는 것 같다.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
