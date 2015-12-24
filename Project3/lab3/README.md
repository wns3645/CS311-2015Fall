

#2015 Fall, KAIST
#CS311, Computer Organization

##Proejct 3

###20130189 김태준
###20130273 박준희


Pipline register에 대한 설명입니다.



1) IF_ID register

 - Instr : structure for instruction (from Proeject 2)
 - NPC : Next PC

2) ID_EX register
 
 - REG1_data, REG2_data : Register에서 읽어 들인 값
 - IMM : Immediate field value
 - Rt_number : Rt 레지스터 번호
 - RD_number : Rd 레지스터 번호
 - Rs_number : Rs 레지스터 번호
 
 *Control signals
 - WB_RegWrite : RegWrite signal
 - WB_MemtoReg : MemtoReg signal
 - M_Branch : Branch signal
 - M_MemRead : MemRead signal
 - M_MemWrite : MemWrite signal
 - EX_RegDst : RegDst signal
 - EX_ALUOp : ALUOp signal
 - EX_ALUSrc : ALUSrc signal


3) EX_MEM register

 - ALU_OUT : result of ALU operation
 - MEM_IN : 메모리에 쓰기 위해 들어가는 값
 - Destination_Register_number : 이름 그대로 변경할 레지스터 번호

 - Zero : Subtraction 을 통해 0인지 아닌지 판별
 - 나머지 시그널 : 위의 Control singal과 같음

4) MEM_WB register

 - ALU_OUT : result of ALU operation
 - MEM_OUT : 메모리에서 읽은 값
 - Destination_Register_number : 이름 그대로 내용을 변경할 레지스터 번호

 - 나머지 시그널 : 위의 Contro signal과 같음


 기타 시그널 : 레지스터에는 따로 저장되지 않고 발생하는 즉시 한 사이클내에서 사용되는 시그널

  - PCsrc : PC값을 어느 것을 선택할 것인지 결정 (MUX control)
  - Jump_signal : Jump인스트럭션인지 판단하여, PC값을 선택 (MUX control)
  - Jump_address : Jump일 경우의 목적 address
  - BR_target : Branch일 경우 목적 address (Branch Predictor에 쓰기 위해서)
  - PCWrite : PC를 업데이틀 할 것인지
  - IF_IDWrite : IF_ID register 를 업데이트 할 것인지 결정
  - IF_FLUSH : IF_ID register에 저장된 시그널을 초기화



 ** 자세한 구조체 및 변수 선언은 util.h 를 참고해주세요!^^
