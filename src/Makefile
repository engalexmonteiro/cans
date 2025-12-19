CC := gcc

# =========================
# Bibliotecas via pkg-config
# =========================
PKG_LIBS := \
	libnm \
	libgps \
	xcb-dpms \
	x11 \
	glib-2.0 \
	dbus-1 \
	dbus-glib-1 \
	glib-2.0 \
	libgps \
	xcb \
	xcb-dpms \
	xext
#to oldest version linux replace libnm to follow
#libnm--util \
#libnm-glib \



PKG_CFLAGS := $(shell pkg-config --libs --cflags $(PKG_LIBS))
PKG_LDFLAGS := $(shell pkg-config --libs $(PKG_LIBS))

# =========================
# Flags gerais
# =========================
CFLAGS  := -Wall -Wextra -O2 $(PKG_CFLAGS)
LDFLAGS := $(PKG_CFLAGS) -liw -lbluetooth -lacpi

TARGET  := cans
PREFIX  := /usr/local
BINDIR  := $(PREFIX)/bin

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

# =========================
# Alvos
# =========================
.PHONY: all clean run help install uninstall check-libs

all: check-libs $(TARGET)

# =========================
# Verificação de libs
# =========================
check-libs:
	@echo " Verificando dependências pkg-config..."
	@for pkg in $(PKG_LIBS); do \
		if ! pkg-config --exists $$pkg; then \
			echo " Dependência '$$pkg' não encontrada (pkg-config)"; \
			exit 1; \
		fi \
	done
	@echo "🔍 Verificando bibliotecas extras (ldconfig)..."
	@for lib in iw bluetooth; do \
		if ! ldconfig -p | grep -q "lib$$lib"; then \
			echo " Biblioteca lib$$lib não encontrada"; \
			exit 1; \
		fi \
	done
	@echo "✅ Todas as dependências estão disponíveis."

# =========================
# Build
# =========================
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) -liw -lm -lacpi

src/%.o: src/%.c
	$(CC) $(LDFLAGS) -c $< -o $@

# =========================
# Execução
# =========================
run: $(TARGET)
	./$(TARGET)

help: $(TARGET)
	./$(TARGET) -h

# =========================
# Instalação (Debian-friendly)
# =========================
install: $(TARGET)
	@echo "📦 Instalando $(TARGET)..."
	@install -d $(DESTDIR)$(BINDIR)
	@install -m 0755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	@echo "✅ Instalação concluída."

uninstall:
	@echo "🗑️ Removendo $(TARGET)..."
	@rm -f $(BINDIR)/$(TARGET)

# =========================
# Limpeza
# =========================
clean:
	rm -f $(TARGET) $(OBJ)
