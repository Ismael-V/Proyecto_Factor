D = [3/4, 1/4]
D_List = [D]

for i in range(0, 128):
    D_List.append(convolution(D_List[i], D))

Carry_P = [sum(L[2::]) for L in D_List]

def Carry_Distribution(num_bits):
    Distro = []

    size = 2*num_bits - 1

    for i in range(0, num_bits):
        Distro.append(Carry_P[i])

    for i in range(1, num_bits):
        Distro.append(Distro[num_bits - i - 1])

    normal = sum(Distro)

    

    return [i/normal for i in Distro]

print(Carry_Distribution(3))