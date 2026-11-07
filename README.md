# VioKey

**Bộ gõ Tiếng Việt thế hệ mới cho Windows 10/11** — kiến trúc siêu nhẹ, tra cứu ký tự `O(1)`, tập trung thuần túy vào chuẩn **Unicode** cùng hai kiểu gõ **Telex & VNI**, đi kèm giao diện **SaaS 2026 (Dark / Light Mode)**.

🌐 **Trang giới thiệu**: Mở file [index.html](file:///e:/Game/Vkey/OpenKey/index.html) trực tiếp trên trình duyệt để xem bản giới thiệu tương tác và bản dùng thử giao diện.

---

## Điểm nổi bật

- **Tinh gọn & Tốc độ (`O(1)`)**: Loại bỏ các bảng mã di sản cũ, tra cứu mảng trực tiếp thay vì cấu trúc cây `std::map` trên luồng phím nóng, xuất thẳng ký tự UTF-16 qua `SendInput`.
- **Giao diện SaaS 2026 (Dark & Light Mode)**: Thiết kế theo phong cách phần mềm hiện đại (Card phân lớp, Segmented Pill, Chip tổ hợp phím, Toggle mượt) hỗ trợ chuyển đổi tức thì giữa chế độ **Tối (Obsidian)** và **Sáng (Alabaster)**.
- **Đầy đủ tính năng cốt lõi**:
  - Kiểu gõ **Telex** và **VNI**, hỗ trợ gõ nhanh (`cc=ch`, `gg=gi`...) và gõ tắt phụ âm đầu/cuối.
  - Kiểm tra chính tả tiếng Việt, tự động khôi phục phím khi gõ từ sai, đặt dấu kiểu mới (`oà`, `uý`).
  - **Gõ tắt (Macro)** với tùy chọn tự động đổi chữ hoa và nhập/xuất danh sách `.txt`.
  - **Nhớ chế độ gõ thông minh** theo từng ứng dụng đang mở.
  - Nhấn đúp `Ctrl` để tạm tắt kiểm tra chính tả, hoặc nhấn đúp `Alt` để tạm ngắt bộ gõ.

---

## Biên dịch (Build)

Yêu cầu **Visual Studio 2022** (hoặc VS2022 Build Tools) với gói C++ Desktop:

```powershell
# Biên dịch ra 1 file thực thi duy nhất: Sources\VioKey\win32\VioKey\x64\Release\VioKey.exe
MSBuild.exe Sources\VioKey\win32\VioKey\VioKey.sln -m -t:Rebuild -p:Configuration=Release -p:Platform=x64
```

## Cấu trúc mã nguồn

- `Sources/VioKey/engine/` — Lõi xử lý tiếng Việt C++ độc lập, quản lý trạng thái âm tiết, chính tả, macro và chuyển ngữ thông minh.
- `Sources/VioKey/win32/VioKey/VioKey/` — Ứng dụng Win32原生, hệ thống hook bàn phím mức thấp, khay hệ thống và bộ dựng giao diện `UITheme` (Dark/Light).
- `index.html` — Trang landing page giới thiệu sản phẩm.

## Giấy phép & Ghi nhận

Phát hành dưới giấy phép **GNU General Public License v3.0 (GPLv3)**.  
Dự án có tham khảo ý tưởng xử lý bộ gõ tiếng Việt từ dự án mã nguồn mở [OpenKey](https://github.com/tuyenvm/OpenKey) của tác giả Mai Vũ Tuyên.
