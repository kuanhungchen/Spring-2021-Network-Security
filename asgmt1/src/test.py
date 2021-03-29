import os
from random import choice, randint


def get_zero():
    return '0'

def get_one():
    return '1'

def get_small():
    bits = randint(1, 10)
    return choice('123456789abcdef') + ''.join([choice('0123456789abcdef') for i in range(bits)])

def get_large():
    bits = randint(90, 100)
    return choice('123456789abcdef') + ''.join([choice('0123456789abcdef') for i in range(bits)])

def cpp_results(a, b):
    res = os.popen('./main {} {}'.format(a, b))
    res = res.read()
    res = res.strip()
    res = res.split()
    try:
        return [res[2], res[5], res[8], res[11], res[14]]
    except:
        return [res[2], res[5], res[8], res[11], 'x']

def python_results(a, b):
    dec_a = int(a, 16)
    dec_b = int(b, 16)

    addition = dec_a + dec_b
    addition = hex(addition).split('x')[-1]
    addition = str(addition)

    subtration = dec_a - dec_b
    neg = subtration < 0
    subtration = hex(subtration).split('x')[-1]
    if neg:
        subtration = '-' + subtration
    subtration = str(subtration)

    multiplication = dec_a * dec_b
    multiplication = hex(multiplication).split('x')[-1]
    multiplication = str(multiplication)

    division = dec_a // dec_b
    division = hex(division).split('x')[-1]
    division = str(division)

    modulo = dec_a % dec_b
    modulo = hex(modulo).split('x')[-1]
    modulo = str(modulo)

    return [addition, subtration, multiplication, division, modulo]

if __name__ == '__main__':
    tests = ['add', 'sub', 'mul', 'div', 'mod']
    check = {'add': 0, 'sub': 0, 'mul': 0, 'div': 0, 'mod': 0}
    for _ in range(100):
        # pass
        a = get_one()
        b = get_one()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_small()
        b = a
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_large()
        b = a
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_zero()
        b = get_one()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_zero()
        b = get_small()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_zero()
        b = get_large()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_one()
        b = get_small()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_one()
        b = get_large()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_small()
        b = get_small()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))
        # pass
        a = get_small()
        b = get_large()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # pass
        a = get_large()
        b = get_large()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # div: 93
        a = get_large()
        b = get_one()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if i == 3 or i == 4:
                print(cr)
                print(pr)
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))

        # div: 21
        a = get_small()
        b = get_one()
        cpp_res = cpp_results(a, b)
        py_res = python_results(a, b)
        for i, (cr, pr) in enumerate(zip(cpp_res, py_res)):
            if i == 3 or i == 4:
                print(cr)
                print(pr)
            if cr != pr:
                check[tests[i]] += 1
                print('--------------------- {} FAIL ---------------------'.format(tests[i]))
                print('a = {}'.format(a))
                print('b = {}'.format(b))
                print('cr = {}'.format(cr))
                print('pr = {}'.format(pr))
    for k,v in check.items():
        print('{}: {}'.format(k, v))
