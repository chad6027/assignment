# assignment

## [1] B+-Tree 
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

### 5. About my experience ( Feedback myself)
* 파일 입출력, read, write 함수들을 제대로 다루어 본 경험이 없어 파일 입출력이 익숙해 지는데 많은 시간을 투자하였습니다.
* full 상태에서 insert를 할 때 기존 Block과 새로운 Block에 entry 분배를 어떻게 하느냐에 따라 print함수의 결과가 달라져서 sample case에 print output과 같은 결과를 내기 위해 많은 시도를 했습니다.
* leaf node와 non leaf node에 각각 insert를 하는 방식이 다른 점이 코딩에 생각보다 많은 어려움을 부여했습니다. 
* 출력이 제가 원하는 대로 되지 않았을 때, bin파일을 갖고 잘못된 부분을 찾는 과정에 상당한 어려움이 있었습니다.
