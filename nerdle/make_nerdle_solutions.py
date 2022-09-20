# DSBYU 2022-08-24. What are all the possible nerdle solutions?
# Two approaches I see:
# - put all the possible character combinations in there, see which ones evaluate, pair out things that aren't allowed (such as leading zeros or standalone zeros on the LHS)
# - Build up the allowable templates to limit the sheer number of combinations and to catch the unallowables on the way in. But this way seems like there are more places of error
# As an exercise, why not try both? Compare both on run time, code quality (understandibility, length, etc). They should eventually get the same solution set.

# from typing import Counter
from collections import Counter
from collections import OrderedDict
import operator
# from signal import pause

operators = ["+", "-", "*", "/"]
operator_count = [0, 0, 0, 0]
all_solutions = []
one_big_solution_string = ""


## The answer template method
single_digits_all         = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
single_digits_except_zero =    [1, 2, 3, 4, 5, 6, 7, 8, 9]

three_digit_numbers = []
for d100 in single_digits_except_zero : # no starting with a 0
    for d10 in single_digits_all :
        for d1 in single_digits_all :
            three_digit_numbers.append(d100 * 100 + d10 * 10 + d1)
print("three_digit_numbers len: " + str(len(three_digit_numbers)))

two_digit_numbers = []
for d10 in single_digits_except_zero : # no starting with a 0
    for d1 in single_digits_all :
        two_digit_numbers.append(d10 * 10 + d1)
print("two_digit_numbers len: " + str(len(two_digit_numbers)))


print("single_digits_all len: "         + str(len(single_digits_all)))
print("single_digits_except_zero len: " + str(len(single_digits_except_zero)))

# # templates
# "123456=8"
# "#o#o##=#" t6a 1018
# "#o##o#=#" t6b  990
# "##o#o#=#" t6c 3223
# "##o###=#" t6d    0
# "###o##=#" t6e  659

# "12345=78"
# "#o#o#=##"  t5a 3376
# "##o##=##"  t5b 6480
# "###o#=##"  t5c  659
# "#o###=##"  t5d    0

# "1234=678"
# "#o##=###"  t4a  659
# "##o#=###"  t4b  659


# Build Template 6A: "#o#o##=#"
solutions_6a = []
for oi1 in [0, 1, 2, 3] :
    for oi2 in [0, 1, 2, 3] :
        for n1 in single_digits_except_zero :
            for n2 in single_digits_except_zero :
                for n3 in two_digit_numbers :
                    for rhs in single_digits_all :
                        my_expression = str(n1) + operators[oi1] + str(n2) + operators[oi2] + str(n3)
                        if (eval(my_expression) == rhs) :
                            solution_str = my_expression + "=" + str(rhs)
                            solutions_6a.append(solution_str)
                            all_solutions.append(solution_str)
                            one_big_solution_string += solution_str
                            operator_count[oi1] = operator_count[oi1] + 1
                            operator_count[oi2] = operator_count[oi2] + 1
print("solutions_6a  #o#o## = #  length " + str(len(solutions_6a)))

# Build Template 6B: "#o##o#=#"
solutions_6b = []
for oi1 in [0, 1, 2, 3] :
    for oi2 in [0, 1, 2, 3] :
        for n1 in single_digits_except_zero :
            for n2 in two_digit_numbers :
                for n3 in single_digits_except_zero :
                    for rhs in single_digits_all :
                        my_expression = str(n1) + operators[oi1] + str(n2) + operators[oi2] + str(n3)
                        if (eval(my_expression) == rhs) :
                            solution_str = my_expression + "=" + str(rhs)
                            solutions_6b.append(solution_str)
                            all_solutions.append(solution_str)
                            one_big_solution_string += solution_str
                            operator_count[oi1] = operator_count[oi1] + 1
                            operator_count[oi2] = operator_count[oi2] + 1
print("solutions_6b  #o##o# = #  length " + str(len(solutions_6b)))

# Build Template 6C: "##o#o#"
solutions_6c = []
for oi1 in [0, 1, 2, 3] :
    for oi2 in [0, 1, 2, 3] :
        for n1 in two_digit_numbers :
            for n2 in single_digits_except_zero :
                for n3 in single_digits_except_zero :
                    for rhs in single_digits_all :
                        my_expression = str(n1) + operators[oi1] + str(n2) + operators[oi2] + str(n3)
                        if (eval(my_expression) == rhs) :
                            solution_str = my_expression + "=" + str(rhs)
                            solutions_6c.append(solution_str)
                            all_solutions.append(solution_str)
                            one_big_solution_string += solution_str
                            operator_count[oi1] = operator_count[oi1] + 1
                            operator_count[oi2] = operator_count[oi2] + 1
print("solutions_6c  ##o#o# = #  length " + str(len(solutions_6c)))

# # Build Template 6D: "##o###=#" - No values
# solutions_6d = []
# for oi1 in [0, 1, 2, 3] :
#     for n1 in two_digit_numbers :
#         for n2 in three_digit_numbers :
#             for rhs in single_digits_all :
#                 my_expression = str(n1) + operators[oi1] + str(n2)
#                 if (eval(my_expression) == rhs) :
#                     solution_str = my_expression + "=" + str(rhs)
#                     solutions_6d.append(solution_str)
#                     all_solutions.append(solution_str)
#                     one_big_solution_string += solution_str
#                     operator_count[oi1] = operator_count[oi1] + 1
# print("solutions_6d  ##o###=#  length " + str(len(solutions_6d))) # LENGTH ZERO
print("solutions_6d  ##o### = #  length 0")

# Build Template 6E: "###o##"
solutions_6e = []
for oi1 in [0, 1, 2, 3] :
    for n1 in three_digit_numbers :
        for n2 in two_digit_numbers :
            for rhs in single_digits_all :
                my_expression = str(n1) + operators[oi1] + str(n2)
                if (eval(my_expression) == rhs) :
                    solution_str = my_expression + "=" + str(rhs)
                    solutions_6e.append(solution_str)
                    all_solutions.append(solution_str)
                    one_big_solution_string += solution_str
                    operator_count[oi1] = operator_count[oi1] + 1
print("solutions_6e  ###o## = #  length " + str(len(solutions_6e)))

##################################################################### 5s

# Build Template 5A: "#o#o#=##"
solutions_5a = []
for oi1 in [0, 1, 2, 3] :
    for oi2 in [0, 1, 2, 3] :
        for n1 in single_digits_except_zero :
            for n2 in single_digits_except_zero :
                for n3 in single_digits_except_zero :
                    for rhs in two_digit_numbers :
                        my_expression = str(n1) + operators[oi1] + str(n2) + operators[oi2] + str(n3)
                        if (eval(my_expression) == rhs) :
                            solution_str = my_expression + "=" + str(rhs)
                            solutions_5a.append(solution_str)
                            all_solutions.append(solution_str)
                            one_big_solution_string += solution_str
                            operator_count[oi1] = operator_count[oi1] + 1
                            operator_count[oi2] = operator_count[oi2] + 1
print("solutions_5a  #o#o# = ##  length " + str(len(solutions_5a)))

# Build Template 5B: "##o##=##"
solutions_5b = []
for oi1 in [0, 1, 2, 3] :
    for n1 in two_digit_numbers :
        for n2 in two_digit_numbers :
            for rhs in two_digit_numbers :
                my_expression = str(n1) + operators[oi1] + str(n2)
                if (eval(my_expression) == rhs) :
                    solution_str = my_expression + "=" + str(rhs)
                    solutions_5b.append(solution_str)
                    all_solutions.append(solution_str)
                    one_big_solution_string += solution_str
                    operator_count[oi1] = operator_count[oi1] + 1
print("solutions_5b  ##o## = ##  length " + str(len(solutions_5b)))

# Build Template 5C: "###o#=##"
solutions_5c = []
for oi1 in [0, 1, 2, 3] :
    for n1 in three_digit_numbers :
        for n2 in single_digits_except_zero :
            for rhs in two_digit_numbers :
                my_expression = str(n1) + operators[oi1] + str(n2)
                if (eval(my_expression) == rhs) :
                    solution_str = my_expression + "=" + str(rhs)
                    solutions_5c.append(solution_str)
                    all_solutions.append(solution_str)
                    one_big_solution_string += solution_str
                    operator_count[oi1] = operator_count[oi1] + 1
print("solutions_5c  ###o# = ##  length " + str(len(solutions_5c)))

# # Build Template 5D: "#o###=##" LENTH ZERO
# solutions_5d = []
# for oi1 in [0, 1, 2, 3] :
#     for n1 in single_digits_except_zero:
#         for n2 in three_digit_numbers :
#             for rhs in two_digit_numbers :
#                 my_expression = str(n1) + operators[oi1] + str(n2)
#                 if (eval(my_expression) == rhs) :
#                     solution_str = my_expression + "=" + str(rhs)
#                     solutions_5d.append(solution_str)
#                     all_solutions.append(solution_str)
#                     one_big_solution_string += solution_str
#                     operator_count[oi1] = operator_count[oi1] + 1
# print("solutions_5d  #o### = ##  length " + str(len(solutions_5d)))
print("solutions_5d  #o### = ##  length 0")

##################################################################### 4s

# Build Template 4A: "#o##=###"
solutions_4a = []
for oi1 in [0, 1, 2, 3] :
    for n1 in single_digits_except_zero :
        for n2 in two_digit_numbers :
            for rhs in three_digit_numbers :
                my_expression = str(n1) + operators[oi1] + str(n2)
                if (eval(my_expression) == rhs) :
                    solution_str = my_expression + "=" + str(rhs)
                    solutions_4a.append(solution_str)
                    all_solutions.append(solution_str)
                    one_big_solution_string += solution_str
                    operator_count[oi1] = operator_count[oi1] + 1
print("solutions_4a  #o## = ###  length " + str(len(solutions_4a)))

# Build Template 4B: "##o#=###"
solutions_4b = []
for oi1 in [0, 1, 2, 3] :
    for n1 in two_digit_numbers :
        for n2 in single_digits_except_zero :
            for rhs in three_digit_numbers :
                my_expression = str(n1) + operators[oi1] + str(n2)
                if (eval(my_expression) == rhs) :
                    solution_str = my_expression + "=" + str(rhs)
                    solutions_4b.append(solution_str)
                    all_solutions.append(solution_str)
                    one_big_solution_string += solution_str
                    operator_count[oi1] = operator_count[oi1] + 1
print("solutions_4b  ##o# = ###  length " + str(len(solutions_4b)))

all_counts = dict(Counter(one_big_solution_string))
ordered_counts = OrderedDict(sorted(all_counts.items(), key=operator.itemgetter(1), reverse=True))
for x,y in ordered_counts.items() :
    print(x, y)

f1 = open("nerdle/solutions.txt", "w")
for soln in all_solutions :
    f1.write(soln + "\n")
f1.close()

f2 = open("nerdle/counts.txt", "w")
for key, value in ordered_counts.items() :
    mystr = key + ", " + str(value)
    f2.write(mystr + "\n")
f2.close()

print("\ndone")
