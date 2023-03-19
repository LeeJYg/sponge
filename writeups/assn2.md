Assignment 2 Writeup
=============

My name: LeeJooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about [n] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
[wrap 함수의 경우 isn을 더하고 앞 32비트를 빼면 된다. unwrap의 경우 n의 wrapping 여부를 판단해야 하는데, 이때 checkpoint를 wrapping한 값을 빼서 min 값을 구하고, 그 값에 checkpoint를 다시 더해 n의 abs_seqno를 구한다. 다만 ret값은 min이 음수가 가능한 int32_t인 탓에 음수일 수 있어서 if, else 문으로 각각 나눠 wrapping이 됐을 경우 2^32을 더해 반환하도록 구현했다.

TCPReceiver에서는 우선 isn 값을 받아 저장할 변수가 필요하므로 private member로 _isn을 추가했다. 또한 tcp segment가 SYN flag를 가지고 왔을 때 _isn 값을 초기화하여 저장했다. 이에 더해 checkpoint 역시 저장해야 하므로 추가하였다.]

Implementation Challenges:
[unwrap 함수에서 어떻게 해야 checkpoint에 가장 가까운 abs_seqno를 반환할 수 있을까 고민이 많았다. 그 결과, 차라리 checkpoint 자체를 wrapping한 다음 이 값을 이용해 abs_seqno를 구하면 가능하지 않을까 생각이 들어 그 아이디어를 이용해 구현했다.

]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
