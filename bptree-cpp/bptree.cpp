#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <queue>
using namespace std;

vector<pair<int, int>> r_search;

class Btree {
public:
	int blockSize;
	int rootBid;
	int nodeNumber;
	int temp;
	int h;
	int zero = 0;
	int half, half_B;
	int B;
	int* zeroBlock;
	int* readBlock;
	string s;
	fstream fs;

	Btree(const char *fileName) {
		fs.open(fileName, ios::in | ios::out | ios::binary);
		s = fileName;
		if (!fs.is_open()) {
			//cout << "open" << endl;
			fs.open(fileName, ios::out | ios::binary);
		}
		else {
			fs.read((char*)&blockSize, sizeof(int));
			fs.read((char*)&rootBid, sizeof(int));
			fs.read((char*)&h, sizeof(int));
			nodeNumber = (blockSize - 4) / 8;

			B = nodeNumber + 1;
			if (B % 2 == 0) {
				half_B = B / 2;
			}
			else
				half_B = B / 2 + 1;

			if ((B + 1) % 2 == 0) {
				half = (B + 1) / 2;
			}
			else
				half = (B + 1) / 2 + 1;


			readBlock = new int[blockSize / 4];
			zeroBlock = new int[blockSize / 4];
			memset( &(*zeroBlock), 0, blockSize);

		}
	};

	Btree(const char *fileName, const char *textName) {
		fs.open(fileName, ios::in | ios::binary);

		fs.seekp(8, ios::beg);
		fs.write((char *)&h, sizeof(int));

		fs.close();
	};

	~Btree() {
		fs.close();
	}

	void makeNewBlock() {
		fs.seekp(0, ios::end);
		fs.write((char*)zeroBlock, blockSize);
	}

	void makeBlockZero(int blockNumber) {
		fs.seekp(12 + (blockNumber - 1) * blockSize, ios::beg);
		fs.write((char*)zeroBlock, blockSize);
	}

	void generate(int blockSize) {
		fs.close();
		fs.open(s.c_str(), ios::out | ios::binary);
		temp = 1;
		fs.write((char *)&blockSize, sizeof(int));
		fs.write((char *)&temp, sizeof(int));
		fs.write((char *)&temp, sizeof(int));

	}

	void insert(int key, int rid) {
		fs.seekg(0, ios::end);
		if (fs.tellg() == 12) {
			fs.seekp(12, ios::beg);

			makeNewBlock();

			fs.seekp(12, ios::beg);
			fs.write((char*)&key, sizeof(int));
			fs.write((char*)&rid, sizeof(int));

		}
		else {
			if (rootBid == 1) {
				insertLeafNode(key, rid, 1);
			}
			else {
				int leafBlockNumber = findLeafBlock(key);
				insertLeafNode(key, rid, leafBlockNumber);
			}
		}

		//cout << "insert < " << key << " > success" << endl;

	};

	void insertLeafNode(int key, int rid, int blockNumber) {

		int start = 12 + (blockNumber - 1) * blockSize;
		vector<pair<int, int>> v;

		fs.seekg(start, ios::beg);

		fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
		for (int i = 0; i < nodeNumber; i++) {
			if (*(readBlock + (2 * i)) != 0) {
				v.push_back({ *(readBlock + (2 * i)) , *(readBlock + (2 * i + 1)) });
			}
		}

		makeBlockZero(blockNumber);

		fs.seekp(8 + (blockNumber)* blockSize, ios::beg);
		fs.write((char*)(readBlock + (blockSize / 4 - 1)), sizeof(int)); //이어지는 Block

		// full이 아닐 때
		if (v.size() != nodeNumber) {

			v.push_back({ key ,rid });
			sort(v.begin(), v.end());
			fs.seekp(start, ios::beg);
			for (int i = 0; i < v.size(); i++) {
				fs.write((char*)&v[i].first, sizeof(int));
				fs.write((char*)&v[i].second, sizeof(int));
			}

		}

		// full 일 때
		else {
			fs.seekg(0, ios::end);
			int newblockNumber = ((int)fs.tellg() - 12) / blockSize + 1;
			makeNewBlock();

			v.push_back({ key ,rid });
			sort(v.begin(), v.end());

			// 원래 있던 block에 insert
			fs.seekp(start, ios::beg);
			for (int i = 0; i < half_B; i++) {
				fs.write((char*)&v[i].first, sizeof(int));
				fs.write((char*)&v[i].second, sizeof(int));
			}

			fs.seekp(8 + (blockNumber)* blockSize, ios::beg);
			fs.write((char*)&newblockNumber, sizeof(int)); //이어지는 Block

			// 새로 생긴 block
			start = 12 + (newblockNumber - 1) * blockSize;
			fs.seekp(start, ios::beg);
			for (int i = half_B; i < v.size(); i++) {
				fs.write((char*)&v[i].first, sizeof(int));
				fs.write((char*)&v[i].second, sizeof(int));
			}

			fs.seekp(8 + (newblockNumber)* blockSize, ios::beg);
			fs.write((char*)(readBlock + (blockSize / 4 - 1)), sizeof(int)); //이어지는 Block


			int parentBlock = findParentBlock(v[0].first, blockNumber);
			insertNonLeafNode(v[half_B].first, newblockNumber, parentBlock);
		}
	}

	void insertNonLeafNode(int key, int rid, int blockNumber) {
		if (rootBid == 1) {
			makeNewBlock();
			fs.seekp(12 + 2 * blockSize, ios::beg);
			temp = 1;
			fs.write((char*)&temp, sizeof(int));
			fs.write((char*)&key, sizeof(int));
			++temp;
			fs.write((char*)&temp, sizeof(int));

			++temp;
			fs.seekp(4, ios::beg);
			fs.write((char*)&temp, sizeof(int));
			rootBid = temp;
		}
		else {
			// block이 full인지 확인하기 위해 block 정보를 가져옴
			vector<int> v;
			vector<int> pointer;

			int start = 12 + (blockNumber - 1) * blockSize;
			fs.seekg(start, ios::beg);
			fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
			for (int i = 0; i < blockSize / 4; i++) {
				if (i % 2 == 0) {
					if (*(readBlock + i) != 0) {
						pointer.push_back(*(readBlock + i));
						continue;
					}
					else
						break;
				}
				else {
					if (*(readBlock + i) != 0)
						v.push_back(*(readBlock + i));
					else
						break;
				}
			}


			// 일단 새로운 값 insert
			int here = v.size() + 1;

			for (int i = 0; i < v.size(); i++) {
				if (v[i] > key) {
					here = i;
					break;
				}
			}
			if (here == v.size() + 1) {
				v.push_back(key);
				pointer.push_back(rid);
			}
			else {
				v.insert(v.begin() + here, key);
				pointer.insert(pointer.begin() + here + 1, rid);
			}



			//full이 아니라면
			if (v.size() <= nodeNumber) {

				fs.seekp(start, ios::beg);
				fs.write((char*)&pointer[0], sizeof(int));
				for (int i = 1; i <= v.size(); i++) {
					fs.write((char*)&v[i - 1], sizeof(int));
					fs.write((char*)&pointer[i], sizeof(int));
				}
			}
			//full 이라면
			else {
				// 기존 블럭에 초기화 후 일단 B / 2만큼 넣음
				makeBlockZero(blockNumber);
				fs.seekp(start, ios::beg);
				fs.write((char*)&pointer[0], sizeof(int));
				for (int i = 1; i < half; i++) {
					fs.write((char*)&v[i - 1], sizeof(int));
					fs.write((char*)&pointer[i], sizeof(int));
				}



				fs.seekg(0, ios::end);
				int newBlockNumber = ((int)fs.tellg() - 12) / blockSize + 1; // 같은 레벨에 새로운 block 하나 더
				makeNewBlock();

				fs.seekg(12 + (newBlockNumber - 1) * blockSize, ios::beg);
				fs.write((char*)&pointer[half], sizeof(int));
				for (int i = half; i < v.size(); i++) {
					fs.write((char*)&v[i], sizeof(int));
					fs.write((char*)&pointer[i + 1], sizeof(int));
				}

				// 내가 root라면
				if (blockNumber == rootBid) {

					//새로운 root가 될 block 생성 후 채워넣기
					fs.seekg(0, ios::end);
					int newRootBlockNumber = ((int)fs.tellg() - 12) / blockSize + 1;

					makeNewBlock();
					fs.seekp(12 + (newRootBlockNumber - 1) * blockSize, ios::beg);
					fs.write((char*)&blockNumber, sizeof(int));
					fs.write((char*)&v[half - 1], sizeof(int));
					fs.write((char*)&newBlockNumber, sizeof(int));

					// header update

					fs.seekp(4, ios::beg);
					fs.write((char*)&newRootBlockNumber, sizeof(int));
					fs.write((char*)&(++h), sizeof(int));
					rootBid = newRootBlockNumber;
					//cout << rootBid << endl;
				}
				// parent가 root가 아니라면
				else {
					// parent를 찾자!
					int parentBlockNumber = findParentBlock(key, blockNumber);

					insertNonLeafNode(v[half - 1], newBlockNumber, parentBlockNumber);
				}

			}

		}

	}

	void print(fstream& of) {
		int start = 12 + (rootBid - 1) * blockSize;
		int cur_h = 0, bnSize = 1;
		queue<int> bn;
		queue<int> qkey;
		bn.push(rootBid);


		// non leaf 출력
		while (cur_h < 2) {
			of << "<" << to_string(cur_h++) << ">\n";
			bnSize = bn.size();

			for (int i = 0; i < bnSize; i++) {
				start = 12 + (bn.front() - 1) * blockSize;
				bn.pop();
				fs.seekg(start, ios::beg);
				fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
				for (int i = 0; i < blockSize / 4; i++) {
					if (i % 2 == 0) {
						if (*(readBlock + i) != 0) {
							bn.push(*(readBlock + i));
							continue;
						}
						else
							break;
					}
					else {
						if (*(readBlock + i) != 0) {
							qkey.push(*(readBlock + i));
							continue;
						}
						else
							break;
					}
				}

			}

			while (!qkey.empty()) {
				//cout << qkey.front() << ", ";
				of << to_string(qkey.front()) << ',';

				qkey.pop();
			}
			of.seekp(-1, ios::end);
			of << '\n';

		}
		
		
	};

	int findParentBlock(int key, int blockNumber) {
		int start = 12 + (rootBid - 1) * blockSize;
		int curBN = rootBid, nextBN = rootBid;

		while (nextBN != blockNumber) {
			curBN = nextBN;
			start = 12 + (curBN - 1) * blockSize;
			fs.seekg(start, ios::beg);
			fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
			for (int i = 0; i < nodeNumber; i++) {
				if (*(readBlock + (2 * i + 1)) > key) {
					nextBN = *(readBlock + (2 * i));
					break;
				}
				else if (*(readBlock + (2 * i + 1)) == 0) {
					nextBN = *(readBlock + (2 * i));
					break;
				}
			}
			if (nextBN == curBN) {
				nextBN = *(readBlock + (2 * nodeNumber));
			}
		}

		return curBN;
	}

	int findLeafBlock(int key) {
		int start = 12 + (rootBid - 1) * blockSize;
		int curBN = rootBid, nextBN = rootBid, cur_h = h;
		//cout << "cur_h : " << cur_h << endl;
		while (cur_h--) {

			curBN = nextBN;
			start = 12 + (curBN - 1) * blockSize;
			fs.seekg(start, ios::beg);
			fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
			for (int i = 0; i < nodeNumber; i++) {
				if (*(readBlock + (2 * i + 1)) > key || *(readBlock + (2 * i + 1)) == 0) {
					nextBN = *(readBlock + (2 * i));
					break;
				}

			}

			if (nextBN == curBN) {
				nextBN = *(readBlock + (2 * nodeNumber));
			}

		}

		//cout << "target key : " << key << "\ttarget Block Number : " << nextBN << endl;

		return nextBN;
	};

	int search(int key) {
		int leafBlock = findLeafBlock(key);
		int id;
		int start = 12 + (leafBlock - 1) * blockSize;
		fs.seekg(start, ios::beg);
		fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
		for (int i = 0; i < nodeNumber; i++) {
			if (*(readBlock + (2 * i)) == key) {
				id = *(readBlock + (2 * i + 1));
			}
		}

		return id;
	}; // point search 

	void search(int startRange, int endRange) {
		r_search.clear();
		int curBlock = findLeafBlock(startRange);
		bool finding = true;
		while (finding) {
			int start = 12 + (curBlock - 1) * blockSize;
			fs.seekg(start, ios::beg);
			fs.read((char*)readBlock, blockSize); // block 단위로 한번에 읽음
			for (int i = 0; i < nodeNumber; i++) {
				if (*(readBlock + (2 * i)) >= startRange && *(readBlock + (2 * i)) <= endRange) {
					r_search.push_back({ *(readBlock + (2 * i)) , *(readBlock + (2 * i + 1)) });
					continue;
				}
				if (*(readBlock + (2 * i)) > endRange) {
					finding = false;
					break;
				}
			}

			curBlock = *(readBlock + (blockSize / 4) - 1);
			if (curBlock == 0)
				finding = false;

		}


	}; // range searc
};


int main(int argc, char* argv[]) {
	char command = argv[1][0];
	Btree* newBtree = new Btree(argv[2]);
	string s, tmp_s, output;

	int t, key, id, here = 0;
	fstream fs, wt;

	switch (command) {
	case 'c':
		s = argv[3];
		t = atoi(s.c_str());
		newBtree->generate(t);
		break;
	case 'i':
		s = argv[3];
		fs.open(s.c_str(), ios::in);
		while (getline(fs, s)) {

			if (s.empty()) {
				continue;
			}

			for (int i = 0; i < s.size(); i++) {
				if (s[i] == ',')
					here = i;
			}
			key = atoi(s.substr(0, here).c_str());
			id = atoi(s.substr(here + 1, s.size() - (here + 1)).c_str());
			newBtree->insert(key, id);

		}

		break;
	case 's':
		s = argv[3];
		output = argv[4];
		fs.open(s.c_str(), ios::in);
		wt.open(output.c_str(), ios::out);
		while (getline(fs, s)) {

			if (s.empty()) {
				continue;
			}

			key = atoi(s.c_str());
			if (key == 0)
				break;
			id = newBtree->search(key);
			s += "," + to_string(id);
			wt << s << '\n';
		}
		break;
	case 'r':
		s = argv[3];
		output = argv[4];
		fs.open(s.c_str(), ios::in);
		wt.open(output.c_str(), ios::out);
		while (getline(fs, s)) {

			if (s.empty()) {
				continue;
			}


			for (int i = 0; i < s.size(); i++) {
				if (s[i] == ',')
					here = i;
			}

			newBtree->search(atoi(s.substr(0, here).c_str()), atoi(s.substr(here + 1, s.size() - (here + 1)).c_str()));

			for (int i = 0; i < r_search.size(); i++) {
				wt << to_string(r_search[i].first) << ',' << to_string(r_search[i].second) << '\t';
			}
			wt.seekp(-1, ios::end);
			wt << '\n';
			r_search.clear();
		}




		break;
	case 'p':
		output = argv[3];
		wt.open(output.c_str(), ios::out);
		newBtree->print(wt);
		break;
	}



	return 0;
}