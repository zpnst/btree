# B-дерево

Тут вы узнаете о том, что такое B-дерево и зачем оно используется, а также посмотрим как оно себя ведёт при выборе различных начальных параметров.

<img src="https://github.com/user-attachments/assets/3aef780a-4183-43c0-a803-a2cdecf8aa35" width="600">

## Что такое B-дерево

### Вспоминаем бинарное дерево поиска
Я подразумеваю, что читатель понимает что такое Бинарное дерево поиска, если нет, то [клик!](https://gitverse.ru/blog/articles/data/327-postroenie-binarnogo-dereva-poiska-struktura-dannyh-i-osnovnye-algoritmy)

Как нам известно, поиск в бинарном дереве в идеальном случае занимает O(log(2, N)). Идеальный случай означает, что дерево сбалансировано. Если же мы решим перенести значения из отсортированного массива в бинарное дерево, то получим связный список и сложность поиска O(N). Все эти случаи показаны на рисунке 1, хоть и “шакальном”, но очень понятном, мы ведь тут за пониманием!

![Pasted image 20241123230950|400](https://github.com/user-attachments/assets/07293324-a8c2-42a2-9bb1-75f843fe7e8d)

Рисунок 1.
### Разбираемся со сложностью алгоритмов 
На рисунке 2 показан график функции f(N) = log(2, N). Означает это, что при добавлении узлов в обычное бинарное дерево поиска, количество которых выражается в N, сложность поиска будет расти логарифмически. то есть для N --> f(N). Если количество узлов N = 2, то f(N) = 1, если N = 4, то  f(N) = 2, а если N = 65536, то f(N) = 16.

То есть если идеально сбалансированное бинарное дерево имеет целых 65536 узлов, то нам потребуется максимум 16 атомарных операций для нахождения любого из элементов в этом дереве и это очень хорошо! Например, в обычном массиве с линейной сложностью поиска O(N) при N = 65536, f(N) = 65536. Для 65536 элементов в худшем случае придётся сделать 65536 атомарных операций для нахождения элемента, например, если он в самом конце(*задача полного перебора*). 

Надеюсь, разница чувствуется. Так почувствуем же её ещё сильнее на примере B-дерева!

<img src="https://github.com/user-attachments/assets/fbd3df76-8372-46a2-bc04-5f7412aa714d" width="500">

Рисунок 2.
### Наконец-то B-дерево!
B-деревья представляют собой сбалансированные деревья, поэтому время выполнения стандартных операций в них пропорционально их высоте. Но в отличие от бинарных деревьев, которые являются 2-арными, они являются “N-арными”. То есть узлы представляют собой не просто ключ-значение как в бинарных деревьях, а массивы с элементами ключ-значение(*массивы структур*).  

Например:

```C  
struct kv_element {
   int key;
   const char *value;

};

int main(void) {
   const int N = 10;
   struct kv_element B_tree_node[N];
   return 0;
}
```

N - это степень дерева. Если степень B-дерева равна N, то каждый из узлов может иметь максимум N потомков и N - 1 значений(*напомню, что узел - это массив*). Все узлы, кроме корня должны иметь как минимум (N/2) – 1 элементов(*результат деления округляем вверх*). 
  
На самом деле, даже эти параметры могут зависеть от конкретной реализации, так что не будем погружаться в скучные формулировки, которые можно и так прочитать в Википедии, а лучше посмотрим как, самое главное, идейно ведёт себя данная структура данных при разных параметрах, а именно при разной степени дерева N. 

И да, пока мы не перешли дальше стоит заметить, что худшее, что может случится с этим деревом в отличие от бинарного дерева поиска - это превращение не в связный список(рисунок 1), а в массив! Если не вдохновило… то сейчас будут картинки =)
## Игра с параметрами

В данной реализации при запуске программы(*сборке проекта, которая описана в Makefile(кхм... в конце есть все инструкции)*) визуализация B-дерева в формате .png автоматически генерируется в папке images.
Насчёт картинок может сразу же возникнуть вопрос. Зачем нам в каждом узле дополнительная пустая ячейка? 

Ну, извините, это лишь издержки утилиты для визуализации, предназначенной для графов. Дело в том, что одна ячейка(*которых в узле может быть много*), описываемая моей программой в .dot файлах, которые она выплёвывает в папку data, может иметь только одну ссылку(*одного ребёнка*), а детей в B-дереве у узла на один больше, чем значений. Так что все претензии к “graphviz”!
  
### Тест №1 (*10 элемнетов, степень дерева 3*)
Вставим 10 элементов от 1 до 10 со степенью дерева 3

![Pasted image 20241124001353](https://github.com/user-attachments/assets/0cb53bb5-6c76-4bc9-8af6-a710dea9161e)

Пока особо ничего интересного, так как не с чем сравнивать. Так как степень дерева 3 - максимальное количество элементов в узле будет 2, а максимальное количество детей у узла будет 3, что мы и видим на картинке.

### Тест №2 (*10 элемнетов, степень дерева 11*)
Вставим 10 элементов от 1 до 10 со степенью дерева 11.

![Pasted image 20241124001405](https://github.com/user-attachments/assets/7feedce3-25a2-47ae-840d-15dd730a6b8b)

А тут уже интереснее. Элементы те же самые, дерево то же самое, код не менялся, но степень дерева теперь 11! Это означает, что узел данного дерева может содержать максимум 10 элементов, что мы и видим на рисунке. 10 элементов попросту влезли в корневой узел дерева, превратив его в обычный массив!

### Тест №3 (*50 элемнетов, степень дерева 5*)
Вставим 50 элементов от 1 до 50 со степенью дерева 5.

![Pasted image 20241124001305](https://github.com/user-attachments/assets/de012657-ed74-4a59-943d-9ab2aaf6a5b7)

Дерево подросло, 50 элементов!

### Тест №4 (*50 элемнетов, степень дерева 25*)
Вставим 50 элементов от 1 до 50 со степенью дерева 25.

![Pasted image 20241124002330](https://github.com/user-attachments/assets/d809c899-5630-45a8-be25-5bf5e1806d1e)

И ещё один пример! Надеюсь, на этом шаге уже стало немного понятно как примерно формируется B-дерево.

### Тест №5 (*Пример из кода в репозитории*)
Тут мы вставляем 17 произвольных элементов со степенью дерева 3

![Pasted image 20241124001601](https://github.com/user-attachments/assets/ab871ba3-7999-439d-b1d9-066b432772f2)

Красиво же, да?
## И зачем это всё? (*Самое интересное и важное*)

B-деревья(*или их производные*) используются во многих файловых системах и базах данных. Например, существует популярная файловая система Btrfs [клик!](https://en.wikipedia.org/wiki/Btrfs), название говорит само за себя. Btrfs(*B-tree file system/Файловая система B-дерева*). Но мы договорились не углубляться в дебри, а понять идею. Рассмотрим основную идею на примере.

Представим огромную базу данных, ну или же просто некий большой пласт данных(*очень большой*), который представлен в виде бинарного дерева поиска. Пусть в каждом его узле будет ключ - id человека и значение - информация об этом человеке. 

Например так:

```C
struct user_info {
   short int age; // да, для жизни хватит и short int =(, вообще и байта хватит(char), но я не хочу путать, или пугать... да что уж там, даже signed char...
   const char *first_name;
   const char *second_name;
};

struct tree_node {
   int id;
   struct user_info current_info;
};
```  
Повторюсь, это не имеет никакого значения, пишу я это исключительно для лучшего понимания читателем, опираясь на уже знакомые ему нарративы(*надеюсь, что знакомые*). 

Так вот, так как дерево реально огромное, хранить его полностью в оперативной памяти не получится => придётся хранить его на стороннем носителе(*допустим, на жестком диске, скорость доступа к которому гораздо медленнее*). Выходит, обычные бинарные деревья будут требовать от нас log(2, N) операций обращений к внешнему носителю, что недопустимо при больших N в сервисах, которые требуют “быстрого отклика”. B-деревья решают эту проблему.

Например пользователей у нас миллиард, значит высота бинарного дерева будет log(2, 1,000,000,000) = 29.897352853986, округлим до 29. Получается, в самом неудачном случае нам придётся 29 раз обратиться к внешему носителю, к диску. В случае с B-деревом при выборе степени M = 1,001, например, потребуется всего 3 дисковые операции, так как каждый узел может хранить до 1,000 элементов. Сложность будет O(log(M, N)).  O(log(1,001, 1,000,000,000)) = 3, ура! Но почему так? 

Возможно станет понятнее, если осознать, что бинарное дерево - это частный случай B-дерева, или же B-дерево со степенью 2(*над этим стоит задуматься, ещё раз посмотрев на основание логарифма в каждом из этих двух случаев*).

## Размышления 

Что касается реализации таких незаурядных вещей на каком бы это ни было языке программирования, стоить понимать, что сами операции, совершаемые, например, над деревом, понять и написать не так уж и сложно. 

Достаточно знать что такое рекурсия и цикл, а вот “разгрести” последствия выполнения этих операций не так уж и просто, даже сложно. Ведь что "B-дерево", что "Красно-Чёрное дерево", что "AVL-дерево" и другие, требуют выполнения специальных правил для сохранения своего баланса, дабы поддерживать сложность выполнения базовых операций(*поиск, вставка, удаление*). Все эти правила нужно проверять и корректировать дерево в соответствии с ними после выполнения каждой из операций. 

Даже нет, задача не сложная, она нудная. Чтобы написать такое без каких-либо сторонних ресурсов нужно или зазубрить тот или иной код, что смешно, или же просто понимать идею. А чтобы понимать идею не нужно быть гением, нужно просто посвятить этому большое(*реально*) количество времени и нервов.


## Заключение
Сказано было многое, поэтому вместо лишних слов в заключение приведу комментарий пользователя Habr, [клик!](https://habr.com/ru/articles/114154/#comment_3679700)

<img src="https://github.com/user-attachments/assets/8f68baf3-98ce-4b6e-8f05-bd01ab67f23a" width="700">


Ой, да, и ещё один! [клик!](https://habr.com/ru/articles/114154/#comment_3679762)

<img src="https://github.com/user-attachments/assets/30507a9d-5a02-49ac-8ae0-d1f362bd90f1" width="700">


PS:
1. Другие виды деревьев есть в моём другом репозитории [клик!](https://github.com/zpnst/different-trees)
2. Извиняюсь перед всеми, кто хорошо знает английский за комментарии в коде, писал я их без переводчика, так как не хотелось каждый раз переключать раскладку клавиатуры(они очень лексически кривые, думаю)
3. Посмотреть как меняется структура B-дерева по мере его заполнения значениями можно наглядно на сайте, [клик!](https://www.cs.usfca.edu/~galles/visualization/BTree.html)
## Сборка проекта 

Пользователи Windows, что я могу сказать... а впрочем, неважно ...
#### Зависимости

Ubuntu, Debian
```bash
sudo apt install graphviz
```

Fedora, e.t.c
```bash
sudo dnf install graphviz
```

Это всё! 
Нам нужна только утилита, использующаяся для визуализации графов. Всё отсльное на UNIX-подобных системах уже есть!
#### Клонирование и запуск

```bash
git clone git@github.com:zpnst/btree.git
cd btree
make run
```

## Полезные ссылки
1. https://habr.com/ru/companies/otus/articles/459216/
2. https://www.youtube.com/live/mHI-7bCgDtM?si=YkcTwq9Z70Yeu4je
3. https://www.youtube.com/live/qx9Pwy5B_Bg?si=S-H-KKyVfELiq2pO
4. https://habr.com/ru/articles/114154/
5. https://www.cs.usfca.edu/~galles/visualization/BTree.html
