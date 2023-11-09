#include<iostream>
#include"Z2_polynomial.h"
using namespace std;

int main(){

    Z2_poly polinomio1("100101011101");
    Z2_poly polinomio2(2);
    Z2_poly polinomio3 = polinomio1 * polinomio2;

    cout << (string)"(" + polinomio1.to_string() + (string)") * (" + polinomio2.to_string() + (string)") = " + polinomio3.to_string() <<  endl;
    cout << polinomio2.to_binary_representation() << endl;

    return 0;
}