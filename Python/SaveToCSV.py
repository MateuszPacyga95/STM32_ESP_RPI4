import csv

measurements = [
    ["2026-10-30 18:00:00", 23.4],
    ["2026-10-30 18:01:00", 24.1],
    ["2026-10-30 18:02:00", 22.8]
]

# with open('measurements.csv', mode='r', newline='', encoding='utf-8') as file:
#     # writer = csv.writer(file)
#     # # writer.writerow(['timestamp', "temperature"])
#     # writer.writerows(measurements)
#     reader = csv.reader(file)
#     next(reader)
    
#     for row in reader:
#         print(row)
        
        
def readTemperature(file):
    temperature = []
    with open(file, mode='r', newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            if(len(row) < 2):
                continue
            try:
                value = float(row[1])
            except ValueError:
                continue
            temperature.append(value)
    return temperature


    

temps = readTemperature('measurements.csv')
average = sum(temps) / len(temps) if temps else 0
print(temps)
print(average)