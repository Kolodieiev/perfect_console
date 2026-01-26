project = 'Pixeler'
copyright = '2026, Kolodieiev'
author = 'Kolodieiev'
release = '1.0'

import os
from exhale import utils # type: ignore

extensions = [
    'sphinx.ext.viewcode',     # Посилання на вихідний код
    'myst_parser',             # Markdown підтримка
    'breathe',                 # Інтеграція з Doxygen
    'exhale',                  # Генерація файлів документації класів
]

myst_enable_extensions = [
    "colon_fence",      # ::: блоки
    "deflist",          # Definition lists
    "fieldlist",        # Field lists
    "attrs_block",      # Атрибути блоків
]

# Breathe налаштування
breathe_projects = {
    "Pixeler": "../docs/xml/"
}
breathe_default_project = "Pixeler"
breathe_default_members = ('members',)

# Exhale налаштування
exhale_args = {
    # Папка, де будуть створені окремі файли для кожного класу
    "containmentFolder": "./classes",
    
    # Головний файл з деревом API
    "rootFileName": "library_root.rst",
    
    # Назва головної сторінки
    "rootFileTitle": "Класи фреймворку",
    
    # Створити дерево навігації
    "createTreeView": True,
    
    # Ми вже генеруємо XML через make dox_xml
    "exhaleExecutesDoxygen": False,
    
    # Видалити шлях з назв
    "doxygenStripFromPath": "..",
}

# Підтримка Markdown файлів
source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

templates_path = ['_templates']
exclude_patterns = []

language = 'uk'
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_css_files = [
    'custom.css',
]
html_theme_options = {
    'navigation_depth': 4,
    'titles_only': False,
}