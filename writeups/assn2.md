Assignment 2 Writeup
=============

My name: LeeJooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about [21] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:

[wrap 함수의 경우 isn을 더하고 앞 32비트를 빼면 된다. unwrap의 경우 n의 wrapping 여부를 판단해야 하는데, 이때 checkpoint를 wrapping한 값을 빼서 min 값을 구하고, 그 값에 checkpoint를 다시 더해 n의 abs_seqno를 구한다. 다만 ret값은 min이 음수가 가능한 int32_t인 탓에 음수일 수 있어서 if, else 문으로 각각 나눠 wrapping이 됐을 경우 2^32을 더해 반환하도록 구현했다.

TCPReceiver에서는 우선 isn 값을 받아 저장할 변수가 필요하므로 private member로 _isn을 추가했다. 또한 tcp segment가 SYN flag를 가지고 왔을 때 _isn 값을 초기화하여 저장했다. 이에 더해 checkpoint 역시 저장해야 하므로 추가하였다. 이후 unwrap을 통해 abs_seqno을 확인하고, push substring을 통해 tcp segment의 payload를 집어 넣는다. 이렇게 한 이후 checkpoint를 length_in_sequence_space를 호출하여 업데이트한다. 이 과정을 통해 tcp segment가 진행된다.

ankno는 현재 write 되어 있는 byte의 수를 알아야 한다. 이는 stream_out의 bytes_written을 통해 얻을 수 있고, ankno는 이 다음 index를 가리켜야 first unassembled index를 가리키게 되므로 1을 더해준다. 이후 input이 끝났다면 마지막 FIN을 위해 1을 추가로 더해주고 wrapping 하여 반환해준다. wrapping 해야 하는 이유는 ankno가 abs_seqno가 아닌 seqno를 refer해야 하기 때문이다.

window_size는 unassembled되어 있는 index의 넓이이다. 따라서 현재 capacity에서 buffer에 올라와 있는 byte 수만큼을 빼주면 구할 수 있다.]

Implementation Challenges:

[unwrap 함수에서 어떻게 해야 checkpoint에 가장 가까운 abs_seqno를 반환할 수 있을까 고민이 많았다. 그 결과, 차라리 checkpoint 자체를 wrapping한 다음 이 값을 이용해 abs_seqno를 구하면 가능하지 않을까 생각이 들어 그 아이디어를 이용해 구현했다.

isn의 변수를 어떤 타입으로 해야 하는지에 대한 문제도 있었다. 처음에는 단순히 wrapping에 사용되는 값이니 WrappingInt32로 설정하면 된다고 생각했지만, isn 자체가 없는 경우도 생각해야 했고 그 때문에 option을 사용하여 nullopt인 경우를 처리할 수 있게 만들었다. 추가로 isn의 초기화를 어떻게 해야 하는지에 대해 어려움이 있었고, nullopt의 존재를 알게 된 이후에야 간신히 초기화를 실행할 수 있었다.

segment_received의 경우 어떤 기능을 처리해야 하는지에 대해 막막함이 있었다. isn의 경우 syn와 함께 건너온 segno 자체가 isn의 역할을 해줄 수 있기 때문에 구할 수 있었지만, 그 이후 기능은 무엇을 해야 하는지 잘 이해가 되지 않았다. ankno는 written된 byte 수를 구하는 것이 어려웠고, 이후 맨 마지막에 +1을 추가로 해줘야 한다는 것 때문에 오류가 발생했었다.]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
