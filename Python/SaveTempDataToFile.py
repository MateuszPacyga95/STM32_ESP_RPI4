

with open("measurements.txt", "w", encoding = "utf-8") as f:
    f.write("TEMP:23.4\n")
    f.write("TEMP:24.1\n")
    f.write("TEMP:22.8\n")




def parse_temperature(data):
    measurementsInfo = data.strip().split(":")

    if len(measurementsInfo) != 2:
        return None

    measType = measurementsInfo[0]



    if measType == "TEMP":
        try:
            temp = float(measurementsInfo[1])
        except ValueError:
            print("Invalid numeric value")
            return None    
        print(f"Temperature: {temp}")
    else:
        print("Unknown measurement type.")
        temp = None
        

    return temp

temperatures = []


with open("measurements.txt", "r", encoding = "utf-8") as f:
    for line in f:
        result = parse_temperature(line)
        print(f"Returned temperature: {result}")

        if result is not None:
            temperatures.append(result) 


if(temperatures):
    average_temp = sum(temperatures) / len(temperatures)
    print(f"Average temperature: {average_temp:.2f}")
