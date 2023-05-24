Assignment 4 Writeup
=============

My name: Lee JooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about [65] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [0.40, 0.28]

Program Structure and Design of the TCPConnection:
[우선 필요한 private 변수와 함수를 선언하였다. tcp_connection.cc에서 구현해야 하는 함수를 보면 time_since_last_segment_received를 반환해야 하는 함수가 있다. 이 값은 상황에 따라 다르게 사용되므로 이 값을 저장할 _time_since_last_segment_received를 private 변수로 선언하였다. 또한 연결이 active 되어있는지 아닌지에 따라 함수들의 동작이 달라지므로 이 값을 저장할 bool type 변수를 선언하였다. 또한 함수를 작성해나가는 과정에서 _sender의 segment_out 큐를 비워야 할 때가 많이 있었다. 때문에 이 기능을 담당할 함수인 sending_data 함수를 선언하여 사용할 수 있도록 하였다. 이때 sending 과정에서 inbound stream이 outbound stream보다 먼저 끝나는 경우가 있을 수 있으므로 _linger_after_streams_finish 변수를 관리해야 하고, cfg.rt_timeout 값을 이용해 active 변수를 false로 하는 과정도 처리해줘야 한다. 마지막으로 rst가 true로 되어 있는 segment를 peer에게 보내야 할 때가 있으므로 이러한 segment를 보내는 함수 set_rst_to_cut_connection를 선언하여 사용하였다.

sending_data와 set_rst_to_cut_connection 함수부터 설명을 하자면, sending_data 함수의 경우 _sender에서 segment_out에 있는 TCPSegment들을 내보내는 역할을 담당하고 있다. 따라서 이 큐가 빌 때까지 front를 뽑아와 TCPConnection의 segment_out에 집어넣는 역할을 한다. 다만 이렇게 보내는 과정은 TCP의 방법을 따라 _receiver가 ackno를 가지고 있을 때 그 값을 받아 ack과 ackno, window size를 업데이트 해야 하므로 그러한 과정을 while문으로 구현하였다. 또한 _receiver의 segment_out이 먼저 끝이 났을 경우 _linger_after_streams_finish를 false로 업데이트 하고, 보내는 과정에서 time_since_last_segment_received가 정해진 숫자(10 * cfg.rt_timeout)을 넘었을 경우 connection을 끄기 위해 active 값을 false로 설정한다.

set_rst_to_cut_connection의 경우 rst 값이 true로 설정된 segment S를 먼저 만들고 그걸 segment_out에 push함으로 sending을 구현하였다. 또한 이 함수가 호출되었다는 것은 connection 자체에 문제가 생겼다는 것이므로 active 변수를 false로 만들고 _sender와 _receiver에서 set_error 함수를 호출해 문제가 있음을 기록하도록 만들었다. 이 함수는 ~TCPConnection()에서 rst segment를 보낼 때 사용되였다.

remaining_outbound_capacity은 _sender에서 stream_in의 remaining_capacity() 함수를 호출하여 반환한다. bytes_in_flight는 _sender의 bytes_in_flight()를 호출하여 처리하였고, unassembled_bytes는 _receiver에서 unassembled_bytes() 함수를 호출하였다. time_since_last_segment_received는 TCPConnection 클래스 내부의 있는 변수 _time_since_last_segment_received를 반환하게 함으로 처리하였다. tick 함수의 경우, _sender에 있는 tick을 이용하면 시간의 흐름을 TCPConnection이 알 수 있지만, 우선 active가 되어 있지 않을 경우는 tick 함수 자체가 무시되어야 하고, _sender의 consecutive_retransmissions이 지정된 값(TCPConfig::MAX_RETX_ATTEMPTS)를 넘었을 경우에 대한 처리를 여기서 해주게 하였다. 이 함수가 주기적으로 호출되니 retransmission가 일어난 횟수도 주기적으로 체크할 수 있기 때문이다. 문제가 생겼다면 set_rst_to_cut_connection 함수를 호출하고 active 변수를 false로 만들어 연결이 끊기는 것을 표시하도록 한다. 또한 tick이 될 때 시간 체크를 위해 sending_data 함수를 호출하였다.

active 함수는 연결이 active 되어 있는지 아닌지 판단하는 것이니 변수 _active를 반환하면 되는 함수이고, write는 _sender의 stream_in에서 write 함수를 호출하여 처리할 수 있었다. 물론 fill_window로 window를 옮기고 sending_data로 마무리하는 것 역시 write 내에서 수행해야 한다. 다만 write 해야 할 data가 비어 있다면 0을 반환하도록 해야 하고, stream_in().write(data)의 반환값이 write된 byte의 size이므로 이를 이 함수에서도 반환해주어야 한다.

connect는 _sender에게 fill_winodw를 하게 함으로 window를 업데이트하고 sending_data로 _sender의 segment_out에 있는 TCPSegment를 비워서 connection이 이루어지도록 만들었다. end_input_stream 함수도 이와 비슷하지만 _sender의 stream_in().end_input() 함수를 호출해서 input이 끝이 났다는 flag를 저장할 수 있도록 만들어야 한다.

마지막으로, segment_received 함수는 segment를 TCPConnection에서 받았을 때 사용되는 함수다. 이때 TCP의 상태는 수업 시간에 배운 state diagram에 따라 다양한 종류의 state로 나뉠 수 있다. 우선 connection이 active되어 있다는 가정 하에 받은 segment가 rst flag를 가지고 있다면 connection에 문제가 생겼다는 뜻이다. 따라서 connection을 종료한다. 두 번째로 state가 Closed/Listen일 경우는 receiver가 seg를 receive하게 하고 connect 함수를 호출해 connection을 연결한다. 이후 Syn flag가 sending 될 때, 보내지는 seg의 ack값이 있느냐 없느냐에 따라 다르게 처리하도록 만들었다. syn flag를 받는 경우에 있어서는 ack_received, segment_received를 이용하여 처리하였고, established의 경우 _sender와 _receiver가 seg를 receive하고 window를 업데이트하고 segment_out를 비우게 만들었다. 이후 Fin-Wait-1, Fin-Wait-2, Closing, Time-Wait와 같이 connection을 끝내야 하는 state들에 경우 ack_received, segment_received, sending_data를 이용해 segment를 처리하고 _sender의 큐를 관리하도록 만들었다. 또한 상황에 따라 fin이 설정되어 있는 경우는 send_empty_segment를 호출해 connection을 마무리할 수 있도록 만들었다. 그 외의 자잘한 state들의 경우, 수업 시간에 배웠던 것과 같이 sender나 receiver가 receive할 수 있도록 하고 window를 update한 다음, sending_data를 할 수 있도록 처리하였다.]

Implementation Challenges:
[이번 assn의 가장 큰 challenge는 다름 아닌 지금까지 했던 assn 0 ~ 3 전부였다. 이번 assn 4에서는 기존에 만들었던 receiver, sender, 그 밖에 다른 모든 클래스와 함수들을 전부 다 사용하여 segment를 receive했을 때 처리해야 했다. 따라서 이번 assn을 시작하기도 전에 머뭇거리게 만든 것은 그 모든 구조들과 함수, 각 기능들이 무엇을 담당하고 무엇을 인자로 받고 무엇을 반환하는지를 이해해야만 했다는 점이었다. 본 assn을 진행하는 과정에서 절반~6할 정도는 모두 그 부분을 다시 배우고 검토하는 것에 할애해야 했던 것 같다. 물론 코드를 작성할 때도 이전 과정을 다시 찾아가며 어떤 함수가 어느 클래스에 있었는지, 이 함수의 기능이 뭐였는지 등을 다시 확인해야 했었으므로 실제 과제의 7할 정도는 이 복잡한 구조를 이해하는 것에 있었던 것 같다. 이것이 가장 커다란 challenge였다.

그 외에 다른 자잘한 함수들의 경우 한 줄로 끝나는 것들이 많았지만 sending_data의 경우 기존에 없던 함수를 새롭게 작성해야 했고, 또 data를 sending할 때 어느 조건들을 생각해야 했는지 떠올리는 것이 또 하나의 challenge였다. 맨 처음에는 segment 하나만 front에서 뽑아 push를 했는데 이는 전체적으로 이해해야 하는 분량이 너무 많아 헷갈려 발생했던 일이었다. 또한 linger_after_streams_finish 를 이 함수에서 관리해야 한다는 것을 이해하지 못하고 while문만 작성한 채 코드를 돌렸을 때 역시 문제가 있었다. 나중에서야 해당 변수를 따로 관리하는 함수가 없다는 것을 깨닫고 고민한 끝에 작성할 수 있었다. 이와 비슷한 문제는 새롭게 작성한 함수인 set_rst_to_cut_connection에서도 비슷하게 나타났다. 행여 놓친 조건이 있을까 해서 test가 fail이 뜰 때마다 이 함수부터 본 탓에 디버깅에서 가장 많은 시간을 잡아먹은 것이 이 두 함수였던 것 같다.

그 외 오밀조밀한 challenge들이 많았지만 implementation에 있어 가장 중요했던 것은 segment_received를 어떻게 구현해야 할 것인가에 대한 거였다. 우선 수업 시간에 배운 state diagram을 기반으로 해야겠다는 것은 떠올릴 수 있었지만 state의 종류 자체가 워낙 많았던 탓에 각각의 조건을 어떻게 나누고 생각해야 할지 머리가 아팠다. active가 false일 경우엔 무시되어야 하겠지만 그 이후의 값은? _time_since_last_segment_received 변수를 이곳에서 초기화해야 하나? 다른 곳에서 초기화해야 하나? if문으로 작성하는 것이 나을까, switch로 작성하는 것이 나을까, case로 한다면 default 값을 어떤 것으로 잡아야 하나, 그런 다양한 고민들이 이 함수의 시작 부분에 있었다. 이때 생각을 체계화하기 위하여 일단 작성한 다른 함수들을 보았고, 이를 통해 rst가 true로 되어 있는 segment를 받았을 때 어떻게 해야 할지 생각해보자, 라는 것 첫 발걸음으로 설정했다. 이후 assn 0~3를 다시 본 다음 abs_seqno가 _sender에서 0일 경우 아직 connection이 시작되지 않은 것이라 할 수 있겠단 생각이 들었고, 그렇게 if문의 뒤로 else if, else if, else if를 쭉 나열해 필요한 state들을 설정하고 필요한 조건들을 작성하게 만들었다. 전체 코드의 구조화가 가장 절실했던 부분이 이 함수를 작성하는 과정에서였다. 예를 들어 _sender의 stream_in이 끝에 도달했다는 조건이 필요한데, 그에 필요한 bool type 변수를 반환하는 함수가 다른 곳에 있었는가, ack flag를 관리하는 함수가 뭐였는지, 해당 flag는 어디서 가지고 와야 하는지 등 수많은 조건들마다 필요한 함수들이 달랐기 때문에 여러모로 머리가 아팠다. 다만 코드를 작성해나가는 과정에서 전체 state에 대해 if문을 작성할 필요는 없을 것 같단 생각이 들었고, 관리가 필요한 state들에 대해 코드를 작성하고 나머지 부분을 else로 묶어 처리할 수 있었다. 만약 writeup을 코드 작성과 병행하여 했다면 더 많은 내용을 넣을 수 있었을 것이란 아쉬움이 든다.]

Remaining Bugs:
[테스트는 모두 통과했다.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
