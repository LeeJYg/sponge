Assignment 3 Writeup
=============

My name: Lee JooYoung

My POVIS ID: jooyounglee

My student ID (numeric): 20190622

This assignment took me about [31] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
[tcp sender는 tcp segment를 sending한다. 이때 각 segment는 substring을 가지고 있고, substring은 SYN flag와 FIN flag로 cap되어 있다.

tcp sender에는 보내려고 하는 segment를 저장하는 segment_out과 함께 성공적으로 보낸 segment들을 저장하는 segment_ready도 존재해야 한다. ack을 받고 나서야 pop을 해야 하기 때문이다. 또한 timer가 필요한다. retransmission timer가 작동했는지 아닌지를 확인하는 bool 타입 _retransmission_timer_activated이 필요하고, RTO 역할을 할 retransmission_timeout, consectutive_retransmission 함수를 위한 변수가 들어있다. 또한 seqno를 보낼 것과 받을 것, 두 개로 구분하여 next_seqno, rcvd_seqno를 선언했고, flag, window size 등을 헤더 파일에 선언하였다.

우선 실질적으로 segment를 sending하는 과정이 자주 사용될 것 같아 해당 함수를 따로 segment_sending으로 구현했다. next_seqno에서 seqno를 wrap해서 얻어내고 _next_seqno과 _bytes_in_flight를 length_in_sequence_space만큼 더하여 값을 업데이트한다. 이후 타이머를 실행시킨다.

fill_window 함수는 내부 flag를 먼저 확인한 후, 끝까지 보낼 수 있는 경우인지 아닌지를 확인한다. 만약 그렇지 않다면 반복적으로 sending을 수행하게 하는데, stream에 버퍼가 비어있지 않다면 tcp segment를 계속 보낼 수 있도록 한다. 이때 stream에서 더 이상 보낼 것이 없을 때 fin을 설정하는 과정을 추가한다.

ack_received는 abs_ackno를 찾아 조건에 따라 처리한다. 만약 abs_ackno가 next_seqno보다 크다면 이미 보낸 seg들은 전부 ack됐다는 얘기므로 return하고, rcvd_seqno보다 크다면 rcvd_seqno를 abs_ackno로 설정하여 ack된 seg가 어느 정도 있는지 확인할 수 있게 한다. 이후 segment_ready의 seg들을 확인하면서 ack되었음이 확인된 seg는 pop될 수 있도록 한다. 또한 ack되었음이 확인되면 타이머를 초기화해야 하므로 retransmission과 관련된 값들을 초기화 한다. 그와 동시에 pop이 되었음이 확인되었다면 fill window 함수를 호출해 window를 업데이트 하도록 한다.

tick은 timer activated 함수가 true인 경우에 한하여 ms_since_last_tick만큼 timer를 업데이트하도록 한다. 만약 RTO만큼의 시간이 지났고, 아직 ack를 받지 못한 seg가 있는 경우 retransmission을 하도록 segment_ready의 front seg를 추출해내고, RTO를 x2만큼 하여 기다리도록 한다. send_empty_segment는 새롭게 tcp segment를 선언하여 이를 push함으로 구현하였다.]

Implementation Challenges:
[wrapping과 unwrapping을 해야 함을 망각하고 코드를 작성하다 보니 첫 번째 challenge가 생겼다. ack_received에서 처음엔 그냥 받은 ack을 그대로 사용하려고 했다가 오류가 발생했는데, 이유를 생각해 보니 unwrap을 하지 않았음을 깨닫고 이를 활용하여 문제를 해결할 수 있었다. segment_sending에서도 마찬가지로 wrap을 하지 않아 문제가 생겼었다.

또한 전체적으로 필요한 클래스의 구성 요소들을 확실하게 머릿속에 집어넣지 않고 있다보니 어려움이 많았다. segment의 header에 접근하려면 어떻게 해야 하는지, 또 이 값을 접근할 수 있는지 없는지, 접근할 수 없다면 어떻게 값을 얻어야 하는지, 전체적으로 내용이 정리되지 않아 구현상의 어려움이 많았다. 결국 필요한 cc 파일과 hh 파일을 다시 정독하고 나서야 전체적인 설계를 시작할 수 있었던 것 같다.

추가적으로 fill_window 함수를 구현할 때 while문에서 조건을 어떻게 작성해야 하는지 고민이 많았다. 단순히 seqno만 생각해야 할 것이 아니라 stream이 비어있는지 아닌지도 확인했어야 했는데 이를 생각하지 않고 seqno와 window만 생각하다보니 문제가 발생했었다. 당연히 더이상 보낼 것이 없다면 보내지 않아야 하는데 그걸 생각하지 못했으니 당연히 문제가 발생하는 것이었고, 디버깅 과정에서 더 이상 보낼 것이 없는데 while문이 끝나지 않는 것을 확인하였다. 그 이후에야 문제를 수정할 수 있었다.]

Remaining Bugs:
[모든 테스트를 통과했다.]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
