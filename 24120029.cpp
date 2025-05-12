#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

struct Node {
    char digit;
    Node* next;
    Node(char d) : digit(d), next(nullptr) {}
};
//Cấu trúc số nguyên lớn
struct BigInt {
    Node* head;
    bool isNegative;
    BigInt() : head(nullptr), isNegative(false) {}
};

Node* reverse(Node* head);
int compare(BigInt a, BigInt b);
int precedence(char op);
int getLength(Node* head);
void trim(BigInt& num);
void append(Node*& head, char digit);
void prepend(Node*& head, char digit);
bool isSmaller(Node* a, Node* b);
bool isZero(const BigInt& num);
string bigIntToString(BigInt num);
string evaluate(const string& expression);
BigInt add(BigInt a, BigInt b);
BigInt multiply(BigInt a, BigInt b);
BigInt divide(const BigInt& a, const BigInt& b);
BigInt parseBigInt(const string& numStr);
BigInt applyOperator(BigInt a, BigInt b, char op);
BigInt subtract(BigInt a, BigInt b);
BigInt absolute(const BigInt& x);

//Đảo ngược danh sách liên kết đơn
Node* reverse(Node* head) {
    Node* prev = nullptr, * curr = head, * next = nullptr;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

//so sánh 2 số nguyên lớn
bool isSmaller(Node* a, Node* b) {
    int len1 = getLength(a);
    int len2 = getLength(b);
    if (len1 != len2)
        return len1 < len2;
    while (a && b) {
        if (a->digit != b->digit)
            return a->digit < b->digit;
        a = a->next;
        b = b->next;
    }
    return false;
}

//Kiểm tra 1 số nguyên lớn có bằng 0 hay không
bool isZero(const BigInt& num) {
    Node* cur = num.head;
    while (cur) {
        if (cur->digit != '0') return false;
        cur = cur->next;
    }
    return true;
}

//Tính số chữ số của số lớn
int getLength(Node* head) {
    int len = 0;
    while (head) {
        len++;
        head = head->next;
    }
    return len;
}

//Hàm so sánh 2 số nguyên lớn
int compare(BigInt a, BigInt b) {
    if (isSmaller(a.head, b.head)) return -1;
    if (isSmaller(b.head, a.head)) return 1;
    return 0;
}

// Hàm kiểm tra độ ưu tiên của các toán tử
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

//Lưu số theo thứ tự gốc
void append(Node*& head, char digit) {
    Node* newNode = new Node(digit);
    if (!head) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
}

//Lưu số theo thứ tự ngược
void prepend(Node*& head, char digit) {
    Node* newNode = new Node(digit);
    newNode->next = head;
    head = newNode;
}

//Loại bỏ số 0 ở đầu
void trim(BigInt& num) {
    while (num.head && num.head->digit == '0' && num.head->next) {
        Node* temp = num.head;
        num.head = num.head->next;
        delete temp;
    }
}

//Chuyển số nguyên lớn ở dạng linked list thành string
string bigIntToString(BigInt num) {
    string result;
    if (num.isNegative) result += "-";
    Node* temp = num.head;
    while (temp) {
        result += temp->digit;
        temp = temp->next;
    }
    return result;
}

//Tạo bản sao dương của 1 số âm 
BigInt absolute(const BigInt& x) {
    BigInt res = x;
    res.isNegative = false;
    return res;
}

//Chuyển chuỗi thành số nguyên lớn dạng linked list
BigInt parseBigInt(const string& numStr) {
    BigInt result;
    int i = 0;
    if (numStr[0] == '-') {
        result.isNegative = true;
        i = 1;
    }
    for (; i < numStr.length(); ++i) {
        if (isdigit(numStr[i]))
            append(result.head, numStr[i]);
    }
    trim(result);
    return result;
}

//Hàm cộng 2 số nguyên lớn
BigInt add(BigInt a, BigInt b) {

    // a < 0 b > ) -> (-a) + b = b - a
    if (a.isNegative && !b.isNegative) {
        a.isNegative = false;
        return subtract(b, a);
    }

    // a và b cùng dấu, kết quả mang dấu đó
    if (!a.isNegative && b.isNegative) {
        b.isNegative = false;
        return subtract(a, b);
    }

    //nếu cả 2 cùng dấu, kết quả mang dấu đó
    BigInt result;
    result.isNegative = a.isNegative;

    //đảo ngược danh sách để cộng từ hàng đơn vị
    Node* h1 = reverse(a.head);
    Node* h2 = reverse(b.head);
    int carry = 0;//biến nhớ

    //cộng từng cặp chữ số và cộng dồn biến nhớ
    while (h1 || h2 || carry) {
        int sum = carry;
        if (h1) {
            sum += h1->digit - '0';
            h1 = h1->next;
        }
        if (h2) {
            sum += h2->digit - '0';
            h2 = h2->next;
        }
        carry = sum / 10;// tính số nhớ cho lần cộng tiếp theo
        prepend(result.head, (sum % 10) + '0');
    }
    return result;
}

//Hàm trừ 2 số nguyên lớn
BigInt subtract(BigInt a, BigInt b) {
    //nếu a < 0 và b > 0 -> (-a) - b = -(a + b)
    if (a.isNegative && !b.isNegative) {
        b.isNegative = true;
        return add(a, b);
    }

    //nếu a > 0 và b < 0 -> a - (-b) = a + b
    if (!a.isNegative && b.isNegative) {
        b.isNegative = false;
        return add(a, b);
    }

    //cả 2 cùng dấu
    //a < b, đổi chỗ để luôn trừ số lớn hơn, kết quả là âm
    if (isSmaller(a.head, b.head)) {
        swap(a, b);
        a.isNegative = true;
    }
    BigInt result;
    result.isNegative = a.isNegative;

    //đảo ngược hai danh sách để bắt đầu trừ từ hàng đơn vị
    Node* h1 = reverse(a.head);
    Node* h2 = reverse(b.head);
    int borrow = 0;

    //thực hiện phép trừ
    while (h1) {
        int sub = h1->digit - '0' - borrow;

        //nếu h2 còn chữ số thì trừ thêm
        if (h2) {
            sub -= h2->digit - '0';
            h2 = h2->next;
        }

        //nếu kết quả âm, mượn 1
        if (sub < 0) {
            sub += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }

        //thêm chữ số vào đầu danh sách kết quả
        prepend(result.head, sub + '0');
        h1 = h1->next;
    }

    //loại bỏ các số 0 thừa ở đầu kết quả
    while (result.head && result.head->digit == '0' && result.head->next) {
        Node* temp = result.head;
        result.head = result.head->next;
        delete temp;
    }
    return result;
}

//Hàm nhân 2 số nguyên lớn
BigInt multiply(BigInt a, BigInt b) {
    BigInt result;
    //khởi tạo kết quả ban đầu là 0
    result.head = new Node('0');

    //đảo ngược danh sách để nhân chữ số từ hàng đơn vị
    Node* h1 = reverse(a.head);
    Node* h2 = reverse(b.head);

    int pos = 0;//vị trí cần dịch trái

    //duyệt từng chữ số của số hạng thứ 1
    for (Node* i = h1; i; i = i->next, pos++) {
        BigInt temp;

        //thêm các số 0 tương ứng với vị trí dịch trái
        for (int j = 0; j < pos; ++j)
            append(temp.head, '0');
        int carry = 0;

        //nhân chữ số i với từng chữ số của số thứ hai
        for (Node* j = h2; j; j = j->next) {
            int prod = (i->digit - '0') * (j->digit - '0') + carry;
            carry = prod / 10;
            append(temp.head, (prod % 10) + '0');
        }

        //nếu còn dư sau khi nhân hết
        if (carry)
            append(temp.head, carry + '0');

        //đảo ngược lại trước khi cộng vào kết quả
        temp.head = reverse(temp.head);

        //cộng phần tạm vào kết quả chính
        result = add(result, temp);
    }

    //xác định dấu kết quả, âm khi 1 trong 2 là âm
    result.isNegative = a.isNegative ^ b.isNegative;
    return result;
}

//Hàm chia 2 số nguyên lớn
BigInt divide(const BigInt& a, const BigInt& b) {
    //nếu b rỗng hoặc = 0 thì thoát chương trình và báo error
    if (b.head == nullptr || (b.head->digit == '0' && b.head->next == nullptr)) {
        cerr << "Error" << endl;
        exit(EXIT_FAILURE);
    }

    BigInt result;
    //xác định dấu của kết quả, âm nếu 1 trong 2 số là âm
    result.isNegative = (a.isNegative != b.isNegative);

    //xử lý phần dương để dễ thao tác
    BigInt dividend = absolute(a);
    BigInt divisor = absolute(b);
    BigInt current;

    //duyệt từng chỗ số của số bị chia
    Node* ptr = dividend.head;
    while (ptr != nullptr) {
        //thêm chữ số hiện tại vào phần dư đang xét
        append(current.head, ptr->digit);
        trim(current);//loại bỏ số 0 dư ở đầu

        //tính số lần trừ divisor khỏi current
        int count = 0;
        while (compare(current, divisor) >= 0) {
            current = subtract(current, divisor);
            ++count;
        }
        //ghi phần thương vào kết quả
        append(result.head, count + '0');
        ptr = ptr->next;
    }

    //loại bỏ số 0 thừa ở đầu của kết quả
    trim(result);
    //nếu kết quả = 0, gán lại bằng 0
    if (result.head == nullptr)
        append(result.head, '0');
    return result;
}

// Hàm thực thi phép toán giữa hai số lớn
BigInt applyOperator(BigInt a, BigInt b, char op) {
    if (op == '+') return add(a, b);
    if (op == '-') return subtract(a, b);
    if (op == '*') return multiply(a, b);
    if (op == '/') return divide(a, b);
    throw invalid_argument("Error");
}

// Hàm xử lý biểu thức
string evaluate(const string& expression) {

    //lưu trữ các toán hạng số nguyên lớn dạng BigInt
    stack<BigInt> values;
    //lưu trữ các toán tử
    stack<char> operators;

    //kiểm tra 1 ký tự có phải toán tử hợp lệ hay không
    auto isOperator = [](char c) {
        return c == '+' || c == '-' || c == '*' || c == '/';
        };

    //Duyệt từng ký tự của biểu thức
    for (size_t i = 0; i < expression.length(); ++i) {
        
        //bỏ qua khoảng trắng
        if (isspace(expression[i])) continue;

        //bỏ dấu mở ngoặc vào chỗ lưu toán tử
        if (expression[i] == '(') {
            operators.push('(');
        }

        //xử lý khi gặp đóng ngoặc cho tới khi gặp mở ngoặc
        else if (expression[i] == ')') {
            while (!operators.empty() && operators.top() != '(') {
                if (values.size() < 2) throw runtime_error("Error");

                //lấy 2 số và 1 toán tử để tính
                BigInt b = values.top(); values.pop();
                BigInt a = values.top(); values.pop();
                char op = operators.top(); operators.pop();

                //kiểm tra trường hợp chia cho 0
                if (op == '/' && isZero(b)) throw runtime_error("Error");

                //tính kết quả và đưa vào lại stack values
                values.push(applyOperator(a, b, op));
            }

            //kiểm tra xem có dấu mở ngoặc không, nếu không thì báo lỗi
            if (operators.empty()) throw runtime_error("Error");

            //bỏ dấu mở ngoặc khỏi stack để tiếp tục xử lý
            operators.pop();
        }
        //kiểm tra 1 số có phải là số âm hay không
        else if (isdigit(expression[i]) ||
            (expression[i] == '-' && (i == 0 || isOperator(expression[i - 1]) || expression[i - 1] == '('))) {
            size_t j = i;

            // Nếu là số âm, bỏ qua dấu -
            if (expression[j] == '-') ++j;

            //nếu sau dấu - không phải là chữ số thì báo lỗi
            if (j >= expression.length() || !isdigit(expression[j])) {
                throw runtime_error("Error");
            }

            //chuyển chuỗi số string thành BigInt, lưu vào values
            while (j < expression.length() && isdigit(expression[j])) ++j;
            string numStr = expression.substr(i, j - i);
            values.push(parseBigInt(numStr));
            i = j - 1;
        }
        //Khi gặp toán tử, thực hiện các phép toán có độ ưu tiên cao hơn trước hoặc bằng khi đẩy toán tử mới vào stack
        else if (isOperator(expression[i])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(expression[i])) {
                if (values.size() < 2) throw runtime_error("Error");
                BigInt b = values.top(); values.pop();
                BigInt a = values.top(); values.pop();
                char op = operators.top(); operators.pop();
                //Kiểm tra chia cho 0
                if (op == '/' && isZero(b)) throw runtime_error("Error");
                values.push(applyOperator(a, b, op));
            }
            //đưa toán tử hiện tại vào stack lưu trữ
            operators.push(expression[i]);
        }
        //ký tự không hợp lệ
        else {
            throw runtime_error(string("Error") + expression[i]);
        }
    }

    //Khi kết thúc biểu thức, tính các phép toán còn lại
    while (!operators.empty()) {
        if (values.size() < 2) throw runtime_error("Error");
        BigInt b = values.top(); values.pop();
        BigInt a = values.top(); values.pop();
        char op = operators.top(); operators.pop();
        if (op == '/' && isZero(b)) throw runtime_error("Error");
        values.push(applyOperator(a, b, op));
    }

    //sau khi tính xong chỉ còn 1 giá trị trong stack value, đó là kết quả cuối cùng
    if (values.size() != 1) throw runtime_error("Error");

    return bigIntToString(values.top());
}




int main(int argc, char* argv[]) {

    string inputFile = argv[1];
    string outputFile = argv[2];
    //biên dịch: g++ 24120029.cpp -o 24120029
    //chạy chương trình: 24120029 tests.txt output_24120029.txt
    //argv[1] là tests.txt và agrv[2] là output_24120029.txt
    ifstream fin(inputFile);
    ofstream fout(outputFile);

    if (!fin.is_open()) {
        cerr << "Failed to open input file: " << inputFile << endl;
        return 1;
    }
    if (!fout.is_open()) {
        cerr << "Failed to open output file: " << outputFile << endl;
        return 1;
    }

    //đọc file
    string line;
    while (getline(fin, line)) {
        try {
            string result = evaluate(line);
            cout << result << endl;
            fout << result << endl;
        }
        catch (const exception& e) {
            cerr << "Error evaluating expression: " << line << "\n" << e.what() << endl;
            fout << "Error" << endl;
        }
    }
    fin.close();
    fout.close();
    return 0;
}
