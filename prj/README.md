# C++ Naming Conventions

This document outlines the recommended naming conventions for C++ programming. Adhering to these conventions helps improve code readability, maintainability, and consistency across the project.

## 1. General Guidelines
- Use **descriptive names** that convey the purpose of the variable, function, or class.
- Avoid abbreviations unless they are widely understood (e.g., `URL`, `ID`).
- Use **camelCase** or **UpperCamelCase** for multi-word identifiers depending on the context.
- Avoid using underscores (`_`) at the beginning of names to prevent conflicts with system or compiler-specific identifiers.
- Use consistent capitalization and formatting throughout the project.

## 2. Classes and Structs
- Use **UpperCamelCase**.
- Class and struct names should be nouns or noun phrases.

### Examples:
```cpp
class NetworkManager;
struct Point3D;
```

## 3. Variables
- Use **camelCase** for variables.
- Use meaningful names that indicate the purpose of the variable.
- For constants, use **UPPER_CASE_WITH_UNDERSCORES**.

### Examples:
```cpp
int itemCount;
float temperatureInCelsius;
const int MAX_CONNECTIONS = 100;
```

## 4. Functions
- Use **camelCase** for function names.
- Function names should start with a verb and describe the action performed.

### Examples:
```cpp
void initializeSystem();
int calculateSum(int a, int b);
```

## 5. Enumerations
- Use **UpperCamelCase** for enumeration types and **UPPER_CASE_WITH_UNDERSCORES** for enumeration values.
- Prefix enumeration values with the enumeration name or an abbreviation to avoid conflicts.

### Examples:
```cpp
enum class Color {
    RED,
    GREEN,
    BLUE
};

enum Status {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_UNKNOWN
};
```

## 6. Constants and Macros
- Use **UPPER_CASE_WITH_UNDERSCORES** for constants and macros.
- Avoid using macros for constants; prefer `const` or `constexpr` instead.

### Examples:
```cpp
const double PI = 3.14159;
constexpr int MAX_BUFFER_SIZE = 1024;
#define DEBUG_MODE 1
```

## 7. Member Variables
- Use **camelCase** with a prefix `m_` for member variables.

### Examples:
```cpp
class Example {
private:
    int m_count;
    std::string m_name;
};
```

## 8. Global Variables
- Avoid using global variables when possible.
- If necessary, use **g_** as a prefix.

### Examples:
```cpp
int g_applicationState;
```

## 9. Template Parameters
- Use **UpperCamelCase** for template parameter names.
- Prefer single uppercase letters for simple, generic types (e.g., `T`, `U`).

### Examples:
```cpp
template <typename T>
class Vector {
    // ...
};

template <class Key, class Value>
class Map {
    // ...
};
```

## 10. Test Functions
- Use **snake_case** for test function names to clearly indicate their purpose.

### Examples:
```cpp
void test_initialize_system();
void test_calculate_sum();
