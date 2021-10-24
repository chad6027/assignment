# assignment

## B+-Tree 
### 1. Description
B+-Tree 구현 파일입니다. Insertion, Point(exact) search, Range search, Print 기능이 구현되어 있습니다.
### 2. Specifications
* Node<br>
<img width="40%" src="https://user-images.githubusercontent.com/49097337/138549031-57f34c86-7205-4bbd-8c08-f1c44c4d6619.jpg"></img>
* Operations
  * Index Creation
  * Insertion
  * Point(exact) Search
  * Range Search
  * Print (print the root node and its child nodes only)
*B+ Tree Structure <br>
<img width="60%" src="https://user-images.githubusercontent.com/49097337/138550116-ae572687-4daf-46d0-b723-adf60680cedd.jpg"></img>
### 3. Command Interface
1) Index Creation   
   btree.exe c [btree binary file] [block_size]<br><br>
2) Insertion   
   btree.exe i [btree binary file] [records data text file]<br><br>
3) Point(exact) Search   
   btree.exe s [btree binary file] [input text file] [output text file]<br><br>
4) Range Search   
   btree.exe r [btree binary file] [input text file] [output text file]<br><br>
5) Print   
   btree.exe p [btree binary file] [output text file]<br><br>
   
### 4. About My Implementation
1) index creation <br>
ios::out 모드로 file을 다시 열어 초기화 시켜주고 header를 write해주었습니다.<br><br>
2) Insertion <br>
insert 함수 실행하여 key가 insert될 Block Id를 찾아 insertLeafNode 함수에 key, ID, 찾은 Block ID를 넘겨줍니다. <br>
leaf node가 full이 아니라면 insert 후 sorting합니다. <br>
full이라면 split하여 2개의 block에 나눠 넣어줍니다. full인 상태에서 insert를 했기 때문에, 기존에 있던 leaf node의 parentBlock을 찾아 insertNonLeafNode 함수에 기존 leaf node에 있던 (B+1)개의 entry 중 [(B + 1) / 2]번째의 key값과 새로운 Block ID, parentBlock ID를 넘겨줍니다.<br> 
non leaf node인 parentBlock이 full이 아니라면 바로 insert를 합니다.<br>
full이라면 split을 하여 2개의 block에 [(B + 1) / 2]번째의 key값을 제외한 나머지를 insert해줍니다. <br>
이후 현재 Block이 만약 root였다면, 새로운 root Block이 될 Block을 생성하여 [(B + 1) / 2]번째의 key값과, 기존에 있던 Block과 처음 새로 만들었던 Block의 pointer(Block ID)들을 저장하고 header 부분에서 rootBid와 depth를 update해줍니다. <br> root가 아니였다면, 다시 parentBlock을 찾아 재귀적으로 [(B+1) / 2]번째의 key값과 full일 때 새로 만들어준 Block의 ID와 방금 찾은 parentBlock ID로 insertNonLeafNode 함수를 실행합니다. <br>

3) Point(exact) search <br>
rootBlock에서 시작하여 찾으려는 key값이 속하는 Block으로 이동하는 과정을 depth만큼 반복하면 key값이 속하는 leaf node를 찾을 수 있습니다.<br>
찾은 leaf node에서 key값에 해당하는 ID를 return 받아 출력합니다. <br>

4) Range search <br>
[a, b] 범위에 있는 key값의 ID를 찾으려면 Point search 방식으로 a가 속하는 leaf node를 찾아 범위에 속하는 key값들의 ID를 순차적으로 찾았습니다.<br>
찾은 값들을 전역변수로 선언된 vector에 push_back해주고 다음 block으로 가는 pointer를 이용하여 b보다 큰 값이 나오거나 다음 block의 pointer가 0일 때까지 진행합니다.<br>
이후 main함수로 돌아와 벡터에 들어있는 값들을 출력합니다.<br>

5) Print <br>
root block에서부터 key값과 pointer들을 각각 다른 queue에 집어넣어 read가 끝나면 queue에 들어있는 key값들을 출력해주고, 다음 level로 넘어가 queue에 담겨있는 pointer들에 하나씩 접근하여 key값들을 모은 후 출력해주었습니다.
(depth가 2일 때, 즉 <0> level은 non leaf, <1> level은 leaf 같은 상황은 없다는 가정 하에 구현하였습니다.)

### 5. About my experience ( Feedback myself )
* 파일 입출력, read, write 함수들을 제대로 다루어 본 경험이 없어 파일 입출력이 익숙해 지는데 많은 시간을 투자하였습니다.
* full 상태에서 insert를 할 때 기존 Block과 새로운 Block에 entry 분배를 어떻게 하느냐에 따라 print함수의 결과가 달라져서 sample case에 print output과 같은 결과를 내기 위해 많은 시도를 했습니다.
* leaf node와 non leaf node에 각각 insert를 하는 방식이 다른 점이 코딩에 생각보다 많은 어려움을 부여했습니다. 
* 출력이 제가 원하는 대로 되지 않았을 때, bin파일을 갖고 잘못된 부분을 찾는 과정에 상당한 어려움이 있었습니다.

***
## Atmeg128 + uc/os-ii
### 1. Description
ADC의 값을 구해 FND에 정확한 값을 출력, (ADC / 100) + 1의 값(8이상은 8과 동일)과 같은 개수의 LED를 켜주고, 만약 ADC의 값이 미리 설정해둔 기준 값(700) 보다 크다면 Buzzer가 울리는 응용 프로그램을 구현했습니다. Buzzer가 울릴 때, 700~799 - 도 / 800~899 - 미 / 900~999 - 솔 /1000 ~ 1023 - 시 음이 울리도록 설정되어있습니다.<br><br>
### 2. Specifications   
* Task [prioirity]
  * AdcTask [0]
  * ValueDeliverTask [1]
  * BuzzerTask [2]
  * FndTask [3]
  * LedTask [4]
  * FndDisplayTask[5]
<br><br>
### 3. About Task
1. AdcTask [0]   
InitAdc() 함수를 호출하여 ADC 값을 읽기 위한 기본 설정 후 read_adc()함수를 호출하여 ADC 값을 읽어들입니다.   
Mail Box를 이용하여 ValueDeliverTask에게 ADC 값을 전달하고, event flag를 이용하여 3, 4, 5, 6번 Task로부터 flag set이 될 때까지 기다립니다.
   
2. ValueDeliverTask [1]   
Mail Box를 통해 adc_value값을 받고, 받은 값을 다시 Message Queue를 이용하여 3개의 Task에게 전달합니다.
   
3. BuezzerTask[2]   
InitBuzzer() 함수를 호출하여 Buzzer 사용을 위한 기본 설정을 합니다.   
이 때, 아직 ADC 값을 받기 전이므로, Buzzer는 울리지 않게 설정합니다. 또한 실습 때 사용하던 0번 타이머가 이미 사용중이므로, 2번 타이머를 이용하도록 설정합니다.    
Message Queue를 통해 받은 ADC 값이 미리 설정된 값 (700) 보다 크면, critical section에 들어가서
* 700~799 = 도   
* 800~899 = 미   
* 900~999 = 솔   
* 1000~1023 = 시  
각 값에 해당하는 음계를 저장하고 Timer overflow interrupt 비트를 1로 설정하여 Buzzer가 울리도록 설정합니다.   
700보다 작다면, Timer overflow interrupt 비트를 0으로 설정하여 Buzzer가 울리지 않게 설정합니다. 마지막으로 자신의 task number에 해당하는 비트를 event flag에 post합니다.

4. FndTask [3]   
InitFnd() 함수를 호출하여 FND 사용을 위한 설정을 합니다.   
Message Queue를 통해 ADC 값을 받고, critical section에 들어가 안전하게 공유 변수 FndNum에 ADC 값을 저장해준 후 빠져나옵니다.   
마지막으로 자신의 task number에 해당하는 비트를 event flag에 post합니다.
   
5. LedTask [4]   
InitLed() 함수를 호출하여 LED 사용을 위한 설정을 합니다. Message Queue를 통해 ADC 값을 받고, 켜질 LED의 개수를 계산합니다.   
LED의 개수는 **아래 표** 에 따라 설정됩니다. PORTA에 계산 값을 저장하고 자신의 task number에 해당하는 비트를 event flag에 post합니다.
   
|ADC | LED 개수|
|---- | ----|
|0    ~ 99  |  1|
|100  ~ 199 |  2|
|200  ~ 299 |  3|
|300  ~ 399 |  4|
|400  ~ 499 |  5|
|500  ~ 599 |  6|
|600  ~ 699 |  7|
|700  ~     |  8|

6. FndDisplayTask   
critical section에 진입하여 공유 변수 FndNum에 저장된 값을 가져옵니다.   
빠져나온 후 FND에 받은 값을 출력해주고, task number에 해당하는 비트를 event flag에 post합니다.

### 4. How it works   
   
[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/dtJGn26cmLM/0.jpg)](http://www.youtube.com/watch?v=dtJGn26cmLM)   
이미지를 클릭하시면 시연 영상을 볼 수 있습니다.   

### 5. About my experience ( Feedback myself )
과제를 진행하며 두가지 이슈가 발생했습니다.
1. 첫번째 이슈로, 3번 Task인 BuzzerTask와 4번 Task인 FndTask에 Message Queue를 사용하면서 OSTimeDlyHMSM() 함수로 delay시키지 않았을 때 원하는 방향으로 작동하지 않았습니다.   
그 이유는 BuzzerTask가 Block 상태가 되지 않기 때문에 BuzzerTask가 Message Queue가 빌 때 까지 수행된 후에 FndTask, LedTask, FndDisplayTask가 수행됩니다.   
이 때, Message Queue는 이미 빈 상태이기 때문에 FndTask와 LedTask는 OSQPend에서 Block되며 flag를 보내지 못하게 됩니다.   
결과적으로 FndDisplayTask만 무한 루프를 돌며 FndNum에 저장된 값을 출력하게 되는 의도치 않은 결과가 있었습니다.<br><br>
아래는 첫번째 이슈의 Task 수행 과정입니다.   
AdcTask (OSFlagPend Block)   
-> ValueDeliverTask (한번 수행 후 OSMboxPend Block)   
-> BuzzerTask (3번 수행 후 OSQPend에서 Block)   
-> FndTask (수행되자마자 OSQPend에서 Block)   
-> LedTask (수행되자마자 OSQPend에서 Block)   
-> FndDisplayTask (다른 Task들 모두 Block 상태 = 무한루프)   

2. 두번째 이슈는 Task delay 없이 오직 우선순위가 가장 낮은 Task인 FndDisplayTask가 flag를 보냈을 때 AdcTask가 깨어나게 구현했을 경우 정상적으로 작동한다는 것입니다.   
첫번째 이슈와 차이점은 AdcTask가 깨어나는 조건이 3,4,5,6번 Task에게 event flag를 받았을 때라면, 두번째 이슈는 6번 Task만 event flag를 보냈을 때 AdcTask가 Block에서 벗어나는 것입니다.<br>
처음에는 첫번째 이슈와 마찬가지로 두번째 이슈도 BuzzerTask가 ADC 값을 세번 가져가는 현상 때문에 정상 출력이 이루어지면 안된다고 생각했지만 실제로 그렇지 않았습니다.   
두번째 이슈에서는 BuzzerTask가 독식하는 상황이 처음 실행에서만 발생하고 Task들이 한번씩 수행이 된 이후에는 발생되지 않습니다.<br><br>
Task들의 수행 흐름은 이렇습니다.   
AdcTask (OSFlagPend Block)   
-> ValueDeliverTask (한번 수행 후 OSMboxPend Block)    
-> BuzzerTask (3번 수행 후 OSQPend에서 Block)   
-> FndTask (수행되자마자 OSQPend에서 Block)   
-> LedTask (수행되자마자 OSQPend에서 Block)   
-> FndDisplayTask (한번 수행)<br><br>
여기까지는 첫번째 이슈가 데드락에 빠지는 과정과 동일합니다.   
하지만 이 상황에선 AdcTask는 FndDisplayTask의 신호만 받으면 깨어나기 때문에 다른 전개가 펼쳐집니다.<br><br>
FndDisplayTask (OSFlagPost 후 AdcTask가 깨기 때문에 Block)   
-> AdcTask   
-> ValueDeliverTask (Adc 값을 받은 후 OSQPost 3번을 하면서 3, 4, 5번을 Ready list에 삽입)   
-> BuzzerTask (한번 수행 후 OSQPend Block)   
-> FndTask (한번 수행 후 OSQPend Block)   
-> LedTask (한번 수행 후 OSQPend Block)   
-> FndDisplayTask 수행   
-> 이후 정상 수행<br><br>
위와 같은 Task 수행 과정이 진행되어 프로그램이 잘 돌아가는 것을 확인할 수 있었습니다.   
두번째 이슈가 나타났을 때, 정상 작동하는 이유를 파악하지 못해 이해하는 데까지 꽤 많은 시간을 소요했습니다.   
결과적으로 OSQPost함수가 어떻게 구현되어있는지에 대한 이해가 부족했었고, OSQPost 함수의 코드를 보고나서 이해할 수 있었습니다.   
