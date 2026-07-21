# اسم البرنامج الناتج
TARGET = StringDate

# المترجم والخيار الافتراضي
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g

# الملفات الكائنية
OBJS = StringDate.o my_library.o

# الهدف الافتراضي (بناء عادي بدون Sanitizer)
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# بناء مع الفحص (استخدمه عند حدوث أخطاء ذاكرة فقط)
debug: CXXFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

# قواعد البناء للملفات الكائنية
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# تنظيف الملفات المؤقتة
clean:
	rm -f $(OBJS) $(TARGET)

# تشغيل البرنامج مباشرة
run: all
	./$(TARGET)



.PHONY: all debug clean run
